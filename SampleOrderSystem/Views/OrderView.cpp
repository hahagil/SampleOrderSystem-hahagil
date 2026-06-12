#include "OrderView.h"
#include "MainMenuView.h"
#include <iostream>
#include <iomanip>
#include <ctime>

namespace View {

namespace {
const char* statusToStr(Model::OrderStatus s) {
    switch (s) {
        case Model::OrderStatus::RESERVED:  return "RESERVED";
        case Model::OrderStatus::PRODUCING: return "PRODUCING";
        case Model::OrderStatus::CONFIRMED: return "CONFIRMED";
        case Model::OrderStatus::RELEASE:   return "RELEASE";
        case Model::OrderStatus::REJECTED:  return "REJECTED";
    }
    return "UNKNOWN";
}
std::string timeToStr(std::time_t t) {
    std::tm tm{};
    localtime_s(&tm, &t);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tm);
    return buf;
}
} // anonymous namespace

OrderView::OrderView(Controller::OrderController&  orderCtrl,
                     Controller::SampleController& sampleCtrl)
    : orderCtrl_(orderCtrl), sampleCtrl_(sampleCtrl) {}

void OrderView::run() {
    while (true) {
        system("cls");
        std::cout << "=== 주문 관리 ===\n";
        std::cout << "  1. 주문 생성\n";
        std::cout << "  2. 주문 승인 / 거절\n";
        std::cout << "  3. 전체 주문 조회\n";
        std::cout << "  0. 뒤로\n\n선택 > ";

        std::string input = MainMenuView::readLine();
        if      (input == "1") create();
        else if (input == "2") approveOrReject();
        else if (input == "3") listAll();
        else if (input == "0") break;
        else { std::cout << "잘못된 입력입니다.\n"; MainMenuView::pause(); }
    }
}

void OrderView::create() {
    system("cls");
    std::cout << "=== 주문 생성 ===\n\n";

    auto samples = sampleCtrl_.listAll();
    if (samples.empty()) {
        std::cout << "등록된 샘플이 없습니다. 먼저 샘플을 등록하세요.\n";
        MainMenuView::pause(); return;
    }
    std::cout << std::left << std::setw(12) << "ID" << std::setw(18) << "이름" << "재고\n";
    std::cout << std::string(36, '-') << "\n";
    for (const auto& s : samples)
        std::cout << std::left << std::setw(12) << s.sampleId << std::setw(18) << s.name << s.stock << "\n";

    std::cout << "\n샘플 ID   : ";
    std::string sampleId = MainMenuView::readLine();
    if (!sampleCtrl_.findById(sampleId)) {
        std::cout << "[오류] 샘플 ID '" << sampleId << "'를 찾을 수 없습니다.\n";
        MainMenuView::pause(); return;
    }

    std::cout << "주문 수량 : ";
    int qty = 0;
    try { qty = std::stoi(MainMenuView::readLine()); } catch (...) {}
    if (qty <= 0) { std::cout << "[오류] 1 이상의 정수를 입력하세요.\n"; MainMenuView::pause(); return; }

    auto orderId = orderCtrl_.createOrder(sampleId, qty);
    if (orderId.empty()) { std::cout << "[오류] 주문 생성에 실패했습니다.\n"; MainMenuView::pause(); return; }
    std::cout << "\n[완료] 주문 생성: " << orderId << " (RESERVED)\n";
    MainMenuView::pause();
}

void OrderView::approveOrReject() {
    system("cls");
    std::cout << "=== 주문 승인 / 거절 ===\n\n";

    auto reserved = orderCtrl_.listByStatus(Model::OrderStatus::RESERVED);
    if (reserved.empty()) {
        std::cout << "승인 대기 중인 주문이 없습니다.\n";
        MainMenuView::pause(); return;
    }
    printOrders(reserved);

    std::cout << "\n처리할 주문 ID     : ";
    std::string orderId = MainMenuView::readLine();
    std::cout << "승인(A) / 거절(R) : ";
    std::string choice = MainMenuView::readLine();

    if (choice == "A" || choice == "a") {
        if (!orderCtrl_.approve(orderId)) { std::cout << "[오류] 승인에 실패했습니다.\n"; MainMenuView::pause(); return; }
        auto o = orderCtrl_.findById(orderId);
        std::cout << "\n[완료] " << orderId << " → " << statusToStr(o->status) << "\n";
    } else if (choice == "R" || choice == "r") {
        if (!orderCtrl_.reject(orderId)) { std::cout << "[오류] 거절에 실패했습니다.\n"; MainMenuView::pause(); return; }
        std::cout << "\n[완료] " << orderId << " → REJECTED\n";
    } else {
        std::cout << "[오류] A 또는 R을 입력하세요.\n";
    }
    MainMenuView::pause();
}

void OrderView::listAll() {
    system("cls");
    std::cout << "=== 전체 주문 목록 ===\n\n";
    auto orders = orderCtrl_.listAll();
    if (orders.empty()) { std::cout << "주문이 없습니다.\n"; }
    else                { printOrders(orders); }
    MainMenuView::pause();
}

void OrderView::printOrders(const std::vector<Model::Order>& orders) const {
    std::cout << std::left
              << std::setw(12) << "주문ID"
              << std::setw(12) << "샘플ID"
              << std::setw(8)  << "수량"
              << std::setw(12) << "상태"
              << "생성일시\n";
    std::cout << std::string(66, '-') << "\n";
    for (const auto& o : orders)
        std::cout << std::left
                  << std::setw(12) << o.orderId
                  << std::setw(12) << o.sampleId
                  << std::setw(8)  << o.quantity
                  << std::setw(12) << statusToStr(o.status)
                  << timeToStr(o.createdAt) << "\n";
}

} // namespace View
