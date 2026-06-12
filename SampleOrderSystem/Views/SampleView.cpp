#include "SampleView.h"
#include "MainMenuView.h"
#include <iostream>
#include <iomanip>

namespace View {

SampleView::SampleView(Controller::SampleController& ctrl) : ctrl_(ctrl) {}

void SampleView::run() {
    while (true) {
        system("cls");
        std::cout << "=== 샘플 관리 ===\n";
        std::cout << "  1. 샘플 등록\n";
        std::cout << "  2. 샘플 조회\n";
        std::cout << "  3. 샘플 검색\n";
        std::cout << "  0. 뒤로\n\n선택 > ";

        std::string input = MainMenuView::readLine();
        if      (input == "1") registerSample();
        else if (input == "2") listAll();
        else if (input == "3") search();
        else if (input == "0") break;
        else { std::cout << "잘못된 입력입니다.\n"; MainMenuView::pause(); }
    }
}

void SampleView::registerSample() {
    system("cls");
    std::cout << "=== 샘플 등록 ===\n\n";

    std::cout << "샘플 ID            : ";
    std::string id = MainMenuView::readLine();
    if (id.empty()) { std::cout << "[오류] ID를 입력하세요.\n"; MainMenuView::pause(); return; }
    if (ctrl_.findById(id)) { std::cout << "[오류] 이미 존재하는 ID입니다.\n"; MainMenuView::pause(); return; }

    std::cout << "이름               : ";
    std::string name = MainMenuView::readLine();
    if (name.empty()) { std::cout << "[오류] 이름을 입력하세요.\n"; MainMenuView::pause(); return; }

    std::cout << "단위 생산 시간(분/개) : ";
    int upt = 0;
    try { upt = std::stoi(MainMenuView::readLine()); } catch (...) {}
    if (upt <= 0) { std::cout << "[오류] 1 이상의 정수를 입력하세요.\n"; MainMenuView::pause(); return; }

    std::cout << "수율 (0.0 ~ 1.0)   : ";
    double yield = 0.0;
    try { yield = std::stod(MainMenuView::readLine()); } catch (...) {}
    if (yield <= 0.0 || yield > 1.0) { std::cout << "[오류] 0.0 초과 1.0 이하 값을 입력하세요.\n"; MainMenuView::pause(); return; }

    std::cout << "초기 재고          : ";
    int stock = -1;
    try { stock = std::stoi(MainMenuView::readLine()); } catch (...) {}
    if (stock < 0) { std::cout << "[오류] 0 이상의 정수를 입력하세요.\n"; MainMenuView::pause(); return; }

    ctrl_.addSample({id, name, upt, yield, stock});
    std::cout << "\n[완료] 샘플 '" << name << "' 등록되었습니다.\n";
    MainMenuView::pause();
}

void SampleView::listAll() {
    system("cls");
    std::cout << "=== 샘플 목록 ===\n\n";
    auto samples = ctrl_.listAll();
    if (samples.empty()) { std::cout << "등록된 샘플이 없습니다.\n"; }
    else                 { printTable(samples); }
    MainMenuView::pause();
}

void SampleView::search() {
    system("cls");
    std::cout << "검색어 (ID 또는 이름 포함) : ";
    auto results = ctrl_.search(MainMenuView::readLine());
    std::cout << "\n";
    if (results.empty()) std::cout << "검색 결과가 없습니다.\n";
    else { std::cout << "검색 결과 " << results.size() << "건\n\n"; printTable(results); }
    MainMenuView::pause();
}

void SampleView::printTable(const std::vector<Model::Sample>& samples) {
    std::cout << std::left
              << std::setw(12) << "ID"
              << std::setw(18) << "이름"
              << std::setw(14) << "단위시간(분)"
              << std::setw(8)  << "수율"
              << "재고\n";
    std::cout << std::string(58, '-') << "\n";
    for (const auto& s : samples)
        std::cout << std::left
                  << std::setw(12) << s.sampleId
                  << std::setw(18) << s.name
                  << std::setw(14) << s.unitProductionTime
                  << std::setw(8)  << std::fixed << std::setprecision(2) << s.yield
                  << s.stock << "\n";
}

} // namespace View
