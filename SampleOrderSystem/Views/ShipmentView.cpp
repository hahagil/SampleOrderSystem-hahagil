#include "ShipmentView.h"
#include "MainMenuView.h"
#include <iostream>
#include <iomanip>

namespace View {

ShipmentView::ShipmentView(Controller::ShipmentController& shipCtrl)
    : shipCtrl_(shipCtrl) {}

void ShipmentView::run() {
    system("cls");
    std::cout << "=== 출하 처리 ===\n\n";

    auto confirmed = shipCtrl_.listConfirmed();
    if (confirmed.empty()) {
        std::cout << "출하 가능한 주문(CONFIRMED)이 없습니다.\n";
        MainMenuView::pause(); return;
    }

    std::cout << "[ 출하 가능 주문 목록 ]\n";
    std::cout << std::left
              << std::setw(12) << "주문ID"
              << std::setw(12) << "샘플ID"
              << "수량\n";
    std::cout << std::string(32, '-') << "\n";
    for (const auto& o : confirmed)
        std::cout << std::left
                  << std::setw(12) << o.orderId
                  << std::setw(12) << o.sampleId
                  << o.quantity    << "\n";

    std::cout << "\n출하할 주문 ID (0=취소) : ";
    std::string orderId = MainMenuView::readLine();
    if (orderId == "0" || orderId.empty()) return;

    if (shipCtrl_.ship(orderId)) {
        std::cout << "\n[완료] " << orderId << " → RELEASE\n";
    } else {
        std::cout << "\n[오류] 출하 처리에 실패했습니다.\n";
    }
    MainMenuView::pause();
}

} // namespace View
