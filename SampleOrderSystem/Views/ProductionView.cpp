#include "ProductionView.h"
#include "MainMenuView.h"
#include <iostream>
#include <iomanip>

namespace View {

ProductionView::ProductionView(Controller::ProductionController& prodCtrl,
                                Controller::SampleController&     sampleCtrl)
    : prodCtrl_(prodCtrl), sampleCtrl_(sampleCtrl) {}

void ProductionView::run() {
    system("cls");
    std::cout << "=== 생산 현황 ===\n\n";

    auto cur = prodCtrl_.currentOrder();
    if (cur) {
        auto s = sampleCtrl_.findById(cur->sampleId);
        int totalQty = Controller::ProductionController::calcProductionQty(
            cur->quantity, s ? s->yield : 1.0);
        int done = prodCtrl_.producedCount();

        std::cout << "[ 현재 생산 중 ]\n";
        std::cout << "  주문 ID  : " << cur->orderId   << "\n";
        std::cout << "  샘플 ID  : " << cur->sampleId  << "\n";
        std::cout << "  주문 수량: " << cur->quantity   << "개\n";
        std::cout << "  생산 목표: " << totalQty        << "개\n";
        std::cout << "  완료     : " << done            << " / " << totalQty << "\n";

        if (totalQty > 0) {
            int pct = done * 100 / totalQty;
            std::cout << "  진행률   : [";
            int bar = pct / 5;
            for (int i = 0; i < 20; ++i) std::cout << (i < bar ? '#' : ' ');
            std::cout << "] " << pct << "%\n";
        }
    } else {
        std::cout << "현재 생산 중인 주문이 없습니다.\n";
    }

    auto queue = prodCtrl_.queueSnapshot();
    std::cout << "\n[ 대기 큐 — " << queue.size() << "건 ]\n";
    if (queue.empty()) {
        std::cout << "  (비어 있음)\n";
    } else {
        std::cout << std::left << std::setw(12) << "주문ID" << std::setw(12) << "샘플ID" << "수량\n";
        std::cout << std::string(32, '-') << "\n";
        for (const auto& o : queue)
            std::cout << std::left
                      << std::setw(12) << o.orderId
                      << std::setw(12) << o.sampleId
                      << o.quantity << "\n";
    }

    MainMenuView::pause();
}

} // namespace View
