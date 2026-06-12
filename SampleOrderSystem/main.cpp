#include <iostream>
#include <ctime>
#include "Repositories/SampleRepository.h"
#include "Repositories/OrderRepository.h"

int main() {
    std::cout << "=== SampleOrderSystem — Phase 1 Repository Round-trip ===\n\n";

    // ── Write ─────────────────────────────────────────────────────────────────
    {
        Repository::SampleRepository sampleRepo("samples.json");
        Repository::OrderRepository  orderRepo("orders.json");

        Model::Sample s1{"SMP-001", "Alpha Chip",  30, 0.92, 100};
        Model::Sample s2{"SMP-002", "Beta Sensor", 15, 0.85,  50};
        sampleRepo.add(s1);
        sampleRepo.add(s2);

        Model::Order o1{"ORD-0001", "SMP-001", 10, Model::OrderStatus::RESERVED,  std::time(nullptr)};
        Model::Order o2{"ORD-0002", "SMP-002",  5, Model::OrderStatus::CONFIRMED, std::time(nullptr)};
        orderRepo.add(o1);
        orderRepo.add(o2);

        std::cout << "[Write] samples=" << sampleRepo.findAll().size()
                  << "  orders=" << orderRepo.findAll().size() << "\n";
    }

    // ── Reload (simulate restart) ──────────────────────────────────────────────
    {
        Repository::SampleRepository sampleRepo("samples.json");
        Repository::OrderRepository  orderRepo("orders.json");

        auto samples = sampleRepo.findAll();
        auto orders  = orderRepo.findAll();

        std::cout << "[Reload] samples=" << samples.size()
                  << "  orders=" << orders.size() << "\n\n";

        for (const auto& s : samples)
            std::cout << "  Sample  " << s.sampleId << "  " << s.name
                      << "  stock=" << s.stock << "\n";
        for (const auto& o : orders)
            std::cout << "  Order   " << o.orderId << "  sampleId=" << o.sampleId
                      << "  qty=" << o.quantity << "\n";
    }

    std::cout << "\nRound-trip OK.\n";
    return 0;
}
