#include <gtest/gtest.h>
#include "../../SampleOrderSystem/Controllers/OrderController.h"
#include "../../SampleOrderSystem/Controllers/SampleController.h"
#include "../../SampleOrderSystem/Controllers/ProductionController.h"
#include "../Stubs/InMemorySampleRepository.h"
#include "../Stubs/InMemoryOrderRepository.h"

using namespace Controller;
using namespace Model;

class OrderControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        sample.sampleId           = "SMP-001";
        sample.name               = "Alpha Chip";
        sample.unitProductionTime = 0; // instant tick for tests
        sample.yield              = 0.9;
        sample.stock              = 100;
        sampleRepo.add(sample);

        sampleCtrl = std::make_unique<SampleController>(sampleRepo);
        prodCtrl   = std::make_unique<ProductionController>(orderRepo, sampleRepo);
        orderCtrl  = std::make_unique<OrderController>(orderRepo, *sampleCtrl, *prodCtrl);
    }

    Stub::InMemorySampleRepository sampleRepo;
    Stub::InMemoryOrderRepository  orderRepo;
    Model::Sample                  sample;
    std::unique_ptr<SampleController>    sampleCtrl;
    std::unique_ptr<ProductionController> prodCtrl;
    std::unique_ptr<OrderController>      orderCtrl;
};

// Path 1: RESERVED → CONFIRMED (재고 충분)
TEST_F(OrderControllerTest, ApproveWithSufficientStock) {
    auto id = orderCtrl->createOrder("SMP-001", 10);
    ASSERT_FALSE(id.empty());

    EXPECT_TRUE(orderCtrl->approve(id));

    auto order = orderRepo.findById(id);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(OrderStatus::CONFIRMED, order->status);
}

// Path 2: RESERVED → PRODUCING (재고 부족)
TEST_F(OrderControllerTest, ApproveWithInsufficientStock) {
    auto id = orderCtrl->createOrder("SMP-001", 200); // 재고 100 < 200
    ASSERT_FALSE(id.empty());

    EXPECT_TRUE(orderCtrl->approve(id));

    auto order = orderRepo.findById(id);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(OrderStatus::PRODUCING, order->status);
}

// Path 3: RESERVED → REJECTED
TEST_F(OrderControllerTest, RejectOrder) {
    auto id = orderCtrl->createOrder("SMP-001", 5);
    ASSERT_FALSE(id.empty());

    EXPECT_TRUE(orderCtrl->reject(id));

    auto order = orderRepo.findById(id);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(OrderStatus::REJECTED, order->status);
}

// Path 4: 이미 CONFIRMED인 주문은 approve 불가
TEST_F(OrderControllerTest, CannotApproveConfirmedOrder) {
    auto id = orderCtrl->createOrder("SMP-001", 1);
    orderCtrl->approve(id); // RESERVED → CONFIRMED

    EXPECT_FALSE(orderCtrl->approve(id)); // already CONFIRMED
}

// Path 5: 이미 REJECTED인 주문은 approve 불가
TEST_F(OrderControllerTest, CannotApproveRejectedOrder) {
    auto id = orderCtrl->createOrder("SMP-001", 1);
    orderCtrl->reject(id); // RESERVED → REJECTED

    EXPECT_FALSE(orderCtrl->approve(id));
}
