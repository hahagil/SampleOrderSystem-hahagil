#include <iostream>
#include <ctime>
#include "Repositories/SampleRepository.h"
#include "Repositories/OrderRepository.h"
#include "Controllers/SampleController.h"
#include "Controllers/OrderController.h"
#include "Controllers/ProductionController.h"
#include "Controllers/ShipmentController.h"

int main() {
    std::cout << "=== SampleOrderSystem — Phase 3 Controller Scenario ===\n\n";

    Repository::SampleRepository sampleRepo("samples.json");
    Repository::OrderRepository  orderRepo("orders.json");

    Controller::SampleController     sampleCtrl(sampleRepo);
    Controller::ProductionController prodCtrl(orderRepo, sampleRepo);
    Controller::OrderController      orderCtrl(orderRepo, sampleCtrl, prodCtrl);
    Controller::ShipmentController   shipCtrl(orderRepo, sampleCtrl);

    // ── 샘플 등록 ─────────────────────────────────────────────────────────────
    sampleCtrl.addSample({"SMP-001", "Alpha Chip",  30, 0.9, 100});
    sampleCtrl.addSample({"SMP-002", "Beta Sensor", 15, 0.8,   5});
    std::cout << "[Samples] " << sampleCtrl.listAll().size() << " registered\n";

    // ── Path A: 재고 충분 → CONFIRMED ────────────────────────────────────────
    auto id1 = orderCtrl.createOrder("SMP-001", 10);
    orderCtrl.approve(id1);
    auto o1 = orderRepo.findById(id1);
    std::cout << "[Path A] " << id1 << " status=" << (int)o1->status
              << " (expected CONFIRMED=2)\n";

    // ── Path B: 재고 부족 → PRODUCING ────────────────────────────────────────
    auto id2 = orderCtrl.createOrder("SMP-002", 20); // stock=5 < qty=20
    prodCtrl.start();
    orderCtrl.approve(id2);
    auto o2 = orderRepo.findById(id2);
    std::cout << "[Path B] " << id2 << " status=" << (int)o2->status
              << " (expected PRODUCING=1)\n";
    std::cout << "         (production running in background — skip wait)\n\n";

    prodCtrl.stop();

    // ── Path C: REJECTED ──────────────────────────────────────────────────────
    auto id3 = orderCtrl.createOrder("SMP-001", 5);
    orderCtrl.reject(id3);
    auto o3 = orderRepo.findById(id3);
    std::cout << "[Path C] " << id3 << " status=" << (int)o3->status
              << " (expected REJECTED=4)\n";

    std::cout << "\nController scenario OK.\n";
    return 0;
}
