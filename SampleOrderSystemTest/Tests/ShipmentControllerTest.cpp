#include <gtest/gtest.h>
#include "../../SampleOrderSystem/Controllers/ShipmentController.h"
#include "../../SampleOrderSystem/Controllers/SampleController.h"
#include "../Stubs/InMemorySampleRepository.h"
#include "../Stubs/InMemoryOrderRepository.h"

using namespace Controller;
using namespace Model;

class ShipmentControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        sample.sampleId           = "SMP-001";
        sample.name               = "Alpha Chip";
        sample.unitProductionTime = 1;
        sample.yield              = 1.0;
        sample.stock              = 50;
        sampleRepo.add(sample);

        sampleCtrl   = std::make_unique<SampleController>(sampleRepo);
        shipmentCtrl = std::make_unique<ShipmentController>(orderRepo, *sampleCtrl);
    }

    void addOrder(const std::string& id, OrderStatus status, int qty = 10) {
        Order o;
        o.orderId   = id;
        o.sampleId  = "SMP-001";
        o.quantity  = qty;
        o.status    = status;
        o.createdAt = 0;
        orderRepo.add(o);
    }

    Stub::InMemorySampleRepository sampleRepo;
    Stub::InMemoryOrderRepository  orderRepo;
    Model::Sample                  sample;
    std::unique_ptr<SampleController>    sampleCtrl;
    std::unique_ptr<ShipmentController>  shipmentCtrl;
};

// CONFIRMED → RELEASE 전환
TEST_F(ShipmentControllerTest, ShipConfirmedOrder) {
    addOrder("ORD-0001", OrderStatus::CONFIRMED, 10);

    EXPECT_TRUE(shipmentCtrl->ship("ORD-0001"));

    auto o = orderRepo.findById("ORD-0001");
    ASSERT_TRUE(o.has_value());
    EXPECT_EQ(OrderStatus::RELEASE, o->status);
}

// 출하 후 재고 감소 확인
TEST_F(ShipmentControllerTest, ShipReducesStock) {
    addOrder("ORD-0001", OrderStatus::CONFIRMED, 10);

    shipmentCtrl->ship("ORD-0001");

    auto s = sampleRepo.findById("SMP-001");
    ASSERT_TRUE(s.has_value());
    EXPECT_EQ(40, s->stock); // 50 - 10
}

// RESERVED 주문은 출하 불가
TEST_F(ShipmentControllerTest, CannotShipReservedOrder) {
    addOrder("ORD-0001", OrderStatus::RESERVED);
    EXPECT_FALSE(shipmentCtrl->ship("ORD-0001"));
}

// PRODUCING 주문은 출하 불가
TEST_F(ShipmentControllerTest, CannotShipProducingOrder) {
    addOrder("ORD-0001", OrderStatus::PRODUCING);
    EXPECT_FALSE(shipmentCtrl->ship("ORD-0001"));
}

// REJECTED 주문은 출하 불가
TEST_F(ShipmentControllerTest, CannotShipRejectedOrder) {
    addOrder("ORD-0001", OrderStatus::REJECTED);
    EXPECT_FALSE(shipmentCtrl->ship("ORD-0001"));
}

// 존재하지 않는 주문 ID는 출하 불가
TEST_F(ShipmentControllerTest, CannotShipNonExistentOrder) {
    EXPECT_FALSE(shipmentCtrl->ship("ORD-9999"));
}

// listConfirmed: CONFIRMED 주문만 반환
TEST_F(ShipmentControllerTest, ListConfirmedReturnsOnlyConfirmed) {
    addOrder("ORD-0001", OrderStatus::CONFIRMED);
    addOrder("ORD-0002", OrderStatus::RESERVED);
    addOrder("ORD-0003", OrderStatus::RELEASE);

    auto confirmed = shipmentCtrl->listConfirmed();
    ASSERT_EQ(1u, confirmed.size());
    EXPECT_EQ("ORD-0001", confirmed[0].orderId);
}
