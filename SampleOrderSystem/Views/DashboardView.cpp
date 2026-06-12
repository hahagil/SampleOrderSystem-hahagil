#include "DashboardView.h"
#include "MainMenuView.h"
#include <iostream>
#include <iomanip>
#include <map>

namespace View {

DashboardView::DashboardView(Controller::SampleController& sampleCtrl,
                              Controller::OrderController&  orderCtrl)
    : sampleCtrl_(sampleCtrl), orderCtrl_(orderCtrl) {}

void DashboardView::run() {
    system("cls");
    std::cout << "=== 대시보드 ===\n\n";

    auto orders  = orderCtrl_.listAll();
    auto samples = sampleCtrl_.listAll();

    int reserved  = 0, producing = 0, confirmed = 0, released = 0, rejected = 0;
    for (const auto& o : orders) {
        switch (o.status) {
            case Model::OrderStatus::RESERVED:  ++reserved;  break;
            case Model::OrderStatus::PRODUCING: ++producing; break;
            case Model::OrderStatus::CONFIRMED: ++confirmed; break;
            case Model::OrderStatus::RELEASE:   ++released;  break;
            case Model::OrderStatus::REJECTED:  ++rejected;  break;
        }
    }

    std::cout << "[ 주문 현황 ]\n";
    std::cout << "  RESERVED  : " << reserved  << "건\n";
    std::cout << "  PRODUCING : " << producing << "건\n";
    std::cout << "  CONFIRMED : " << confirmed << "건\n";
    std::cout << "  RELEASE   : " << released  << "건\n";
    std::cout << "  REJECTED  : " << rejected  << "건\n";
    std::cout << "  합계      : " << orders.size() << "건\n\n";

    // per-sample 출고/입고 누계
    std::map<std::string, int> shipped, produced;
    for (const auto& o : orders) {
        if (o.status == Model::OrderStatus::CONFIRMED || o.status == Model::OrderStatus::RELEASE)
            shipped[o.sampleId] += o.quantity;
    }

    std::cout << "[ 샘플별 재고 현황 ]\n";
    std::cout << std::left
              << std::setw(12) << "샘플ID"
              << std::setw(18) << "이름"
              << std::setw(10) << "현재재고"
              << "출고누계\n";
    std::cout << std::string(48, '-') << "\n";
    for (const auto& s : samples)
        std::cout << std::left
                  << std::setw(12) << s.sampleId
                  << std::setw(18) << s.name
                  << std::setw(10) << s.stock
                  << shipped[s.sampleId] << "\n";

    MainMenuView::pause();
}

} // namespace View
