#include <gtest/gtest.h>
#include "../../SampleOrderSystem/Controllers/ProductionController.h"
#include "../Stubs/InMemorySampleRepository.h"
#include "../Stubs/InMemoryOrderRepository.h"
#include <chrono>
#include <thread>

using namespace Controller;
using namespace Model;

class ProductionControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        sample.sampleId           = "SMP-001";
        sample.name               = "Alpha Chip";
        sample.unitProductionTime = 0; // unitProductionTime=0 → tick instantly
        sample.yield              = 1.0;
        sample.stock              = 0;
        sampleRepo.add(sample);

        order.orderId   = "ORD-0001";
        order.sampleId  = "SMP-001";
        order.quantity  = 2;
        order.status    = OrderStatus::PRODUCING;
        order.createdAt = 0;
        orderRepo.add(order);

        prodCtrl = std::make_unique<ProductionController>(orderRepo, sampleRepo);
    }

    void TearDown() override {
        prodCtrl->stop();
    }

    Stub::InMemorySampleRepository sampleRepo;
    Stub::InMemoryOrderRepository  orderRepo;
    Model::Sample                  sample;
    Model::Order                   order;
    std::unique_ptr<ProductionController> prodCtrl;
};

// tick 기반 생산 완료 후 CONFIRMED 전환 확인
TEST_F(ProductionControllerTest, CompletesProductionAndConfirms) {
    prodCtrl->start();
    prodCtrl->enqueue(order);

    // unitProductionTime=0이므로 즉시 완료. 최대 1초 대기.
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (std::chrono::steady_clock::now() < deadline) {
        auto o = orderRepo.findById("ORD-0001");
        if (o && o->status == OrderStatus::CONFIRMED) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    auto result = orderRepo.findById("ORD-0001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(OrderStatus::CONFIRMED, result->status);
}

// stop() 호출 시 블록 없이 즉시 종료
TEST_F(ProductionControllerTest, GracefulShutdown) {
    prodCtrl->start();

    auto t0 = std::chrono::steady_clock::now();
    prodCtrl->stop();
    auto elapsed = std::chrono::steady_clock::now() - t0;

    EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(), 500);
}
