#include "../SampleOrderSystem/Repositories/SampleRepository.h"
#include "../SampleOrderSystem/Repositories/OrderRepository.h"
#include <iostream>
#include <iomanip>
#include <map>

namespace {
const char* statusLabel(Model::OrderStatus s) {
    switch (s) {
        case Model::OrderStatus::RESERVED:  return "RESERVED";
        case Model::OrderStatus::PRODUCING: return "PRODUCING";
        case Model::OrderStatus::CONFIRMED: return "CONFIRMED";
        case Model::OrderStatus::RELEASE:   return "RELEASE";
        case Model::OrderStatus::REJECTED:  return "REJECTED";
    }
    return "UNKNOWN";
}
} // anonymous namespace

int main() {
    std::cout << "=== DataMonitor — SampleOrderSystem 현황 ===\n\n";

    Repository::SampleRepository sampleRepo("samples.json");
    Repository::OrderRepository  orderRepo("orders.json");

    auto samples = sampleRepo.findAll();
    auto orders  = orderRepo.findAll();

    // 주문 상태별 집계
    std::map<Model::OrderStatus, int> counts;
    for (const auto& o : orders) counts[o.status]++;

    std::cout << "[ 주문 상태별 집계 ]\n";
    std::cout << "  RESERVED  : " << counts[Model::OrderStatus::RESERVED]  << "건\n";
    std::cout << "  PRODUCING : " << counts[Model::OrderStatus::PRODUCING] << "건\n";
    std::cout << "  CONFIRMED : " << counts[Model::OrderStatus::CONFIRMED] << "건\n";
    std::cout << "  RELEASE   : " << counts[Model::OrderStatus::RELEASE]   << "건\n";
    std::cout << "  REJECTED  : " << counts[Model::OrderStatus::REJECTED]  << "건\n";
    std::cout << "  합계      : " << orders.size() << "건\n\n";

    // 샘플별 재고 현황
    std::map<std::string, int> shipped;
    for (const auto& o : orders)
        if (o.status == Model::OrderStatus::CONFIRMED ||
            o.status == Model::OrderStatus::RELEASE)
            shipped[o.sampleId] += o.quantity;

    std::cout << "[ 샘플별 재고 현황 ]\n";
    std::cout << std::left
              << std::setw(12) << "샘플ID"
              << std::setw(18) << "이름"
              << std::setw(10) << "현재재고"
              << std::setw(10) << "출고누계"
              << "입고누계\n";
    std::cout << std::string(58, '-') << "\n";
    for (const auto& s : samples) {
        int outbound = shipped[s.sampleId];
        std::cout << std::left
                  << std::setw(12) << s.sampleId
                  << std::setw(18) << s.name
                  << std::setw(10) << s.stock
                  << std::setw(10) << outbound
                  << (s.stock + outbound) << "\n";
    }

    if (samples.empty()) std::cout << "  (샘플 없음)\n";
    std::cout << "\n";

    // 최근 주문 목록
    std::cout << "[ 최근 주문 목록 (최대 10건) ]\n";
    std::cout << std::left
              << std::setw(12) << "주문ID"
              << std::setw(12) << "샘플ID"
              << std::setw(8)  << "수량"
              << "상태\n";
    std::cout << std::string(44, '-') << "\n";
    int shown = 0;
    for (int i = static_cast<int>(orders.size()) - 1; i >= 0 && shown < 10; --i, ++shown)
        std::cout << std::left
                  << std::setw(12) << orders[i].orderId
                  << std::setw(12) << orders[i].sampleId
                  << std::setw(8)  << orders[i].quantity
                  << statusLabel(orders[i].status) << "\n";
    if (orders.empty()) std::cout << "  (주문 없음)\n";

    return 0;
}
