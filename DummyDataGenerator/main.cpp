#include "../SampleOrderSystem/Repositories/SampleRepository.h"
#include "../SampleOrderSystem/Repositories/OrderRepository.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cstring>

namespace {
const char* SAMPLE_NAMES[] = {
    "Alpha Chip", "Beta Sensor", "Gamma DRAM", "Delta NAND", "Epsilon SoC",
    "Zeta Logic", "Eta RF IC",  "Theta ADC",  "Iota Power", "Kappa MCU"
};
const double YIELDS[] = { 0.95, 0.90, 0.85, 0.80, 0.75, 0.70, 0.92, 0.88, 0.83, 0.78 };
const int    UNIT_TIMES[] = { 10, 15, 20, 30, 5, 25, 12, 18, 8, 35 };
} // anonymous namespace

int main(int argc, char** argv) {
    int sampleCount = 5;
    int orderCount  = 10;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--samples") == 0 && i + 1 < argc)
            sampleCount = std::atoi(argv[++i]);
        else if (std::strcmp(argv[i], "--orders") == 0 && i + 1 < argc)
            orderCount = std::atoi(argv[++i]);
        else if (std::strcmp(argv[i], "--help") == 0) {
            std::cout << "Usage: DummyDataGenerator [--samples N] [--orders N]\n";
            std::cout << "  --samples N  (default 5, max 10)\n";
            std::cout << "  --orders  N  (default 10)\n";
            return 0;
        }
    }
    if (sampleCount < 1) sampleCount = 1;
    if (sampleCount > 10) sampleCount = 10;
    if (orderCount  < 1) orderCount  = 1;

    Repository::SampleRepository sampleRepo("samples.json");
    Repository::OrderRepository  orderRepo("orders.json");

    // Generate samples
    int added = 0;
    for (int i = 0; i < sampleCount; ++i) {
        char id[12];
        std::snprintf(id, sizeof(id), "DUM-%03d", i + 1);
        if (sampleRepo.findById(id)) continue;  // skip if exists
        Model::Sample s;
        s.sampleId           = id;
        s.name               = SAMPLE_NAMES[i % 10];
        s.unitProductionTime = UNIT_TIMES[i % 10];
        s.yield              = YIELDS[i % 10];
        s.stock              = 50 + (i % 5) * 10;
        sampleRepo.add(s);
        ++added;
    }
    std::cout << "[완료] 샘플 " << added << "개 추가 (DUM-001 ~ DUM-"
              << std::setfill('0') << std::setw(3) << sampleCount << ")\n";

    // Generate orders referencing the dummy samples
    auto allSamples = sampleRepo.findAll();
    if (allSamples.empty()) {
        std::cout << "[오류] 등록된 샘플이 없습니다.\n";
        return 1;
    }

    int orderAdded = 0;
    std::time_t now = std::time(nullptr);
    for (int i = 0; i < orderCount; ++i) {
        auto existing = orderRepo.findAll();
        char id[16];
        std::snprintf(id, sizeof(id), "ORD-%04d", static_cast<int>(existing.size()) + 1);

        const auto& s = allSamples[i % allSamples.size()];
        Model::Order o;
        o.orderId   = id;
        o.sampleId  = s.sampleId;
        o.quantity  = 5 + (i % 4) * 5;
        o.status    = Model::OrderStatus::RESERVED;
        o.createdAt = now - (orderCount - i) * 60;
        orderRepo.add(o);
        ++orderAdded;
    }
    std::cout << "[완료] 주문 " << orderAdded << "개 추가 (RESERVED 상태)\n";

    return 0;
}
