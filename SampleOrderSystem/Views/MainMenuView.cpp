#include "MainMenuView.h"
#include "SampleView.h"
#include "OrderView.h"
#include "DashboardView.h"
#include "ProductionView.h"
#include "ShipmentView.h"
#include <iostream>

namespace View {

MainMenuView::MainMenuView(Controller::SampleController&     sampleCtrl,
                           Controller::OrderController&      orderCtrl,
                           Controller::ProductionController& prodCtrl,
                           Controller::ShipmentController&   shipCtrl)
    : sampleCtrl_(sampleCtrl), orderCtrl_(orderCtrl),
      prodCtrl_(prodCtrl), shipCtrl_(shipCtrl) {}

void MainMenuView::run() {
    while (true) {
        system("cls");
        std::cout << "╔══════════════════════════════════════╗\n";
        std::cout << "║   반도체 샘플 생산주문관리 시스템        ║\n";
        std::cout << "╚══════════════════════════════════════╝\n";
        std::cout << "  1. 샘플 관리\n";
        std::cout << "  2. 주문 관리\n";
        std::cout << "  3. 대시보드\n";
        std::cout << "  4. 생산 현황\n";
        std::cout << "  5. 출하 처리\n";
        std::cout << "  0. 종료\n";
        std::cout << "\n선택 > ";

        std::string input = readLine();

        if      (input == "1") SampleView(sampleCtrl_).run();
        else if (input == "2") OrderView(orderCtrl_, sampleCtrl_).run();
        else if (input == "3") DashboardView(sampleCtrl_, orderCtrl_).run();
        else if (input == "4") ProductionView(prodCtrl_, sampleCtrl_).run();
        else if (input == "5") ShipmentView(shipCtrl_).run();
        else if (input == "0") break;
        else {
            std::cout << "잘못된 입력입니다.\n";
            pause();
        }
    }
}

void MainMenuView::pause() {
    std::cout << "\nEnter를 누르면 계속합니다...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

std::string MainMenuView::readLine() {
    std::string line;
    std::getline(std::cin, line);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    return line;
}

} // namespace View
