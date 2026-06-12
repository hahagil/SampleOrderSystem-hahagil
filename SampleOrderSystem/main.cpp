#include "Repositories/SampleRepository.h"
#include "Repositories/OrderRepository.h"
#include "Controllers/SampleController.h"
#include "Controllers/OrderController.h"
#include "Controllers/ProductionController.h"
#include "Controllers/ShipmentController.h"
#include "Views/MainMenuView.h"
#include <Windows.h>
#include <cstdlib>

// Pointer used by the console control handler (set before registering).
static Controller::ProductionController* g_prodCtrl = nullptr;

// Handles Ctrl+C and console-window close events.
// Stops the production worker cleanly, then exits the process.
static BOOL WINAPI consoleCtrlHandler(DWORD event) {
    if (event == CTRL_C_EVENT || event == CTRL_CLOSE_EVENT) {
        if (g_prodCtrl) g_prodCtrl->stop();
        std::exit(0);
    }
    return FALSE;
}

int main() {
    Repository::SampleRepository sampleRepo("samples.json");
    Repository::OrderRepository  orderRepo("orders.json");

    Controller::SampleController     sampleCtrl(sampleRepo);
    Controller::ProductionController prodCtrl(orderRepo, sampleRepo);
    Controller::OrderController      orderCtrl(orderRepo, sampleCtrl, prodCtrl);
    Controller::ShipmentController   shipCtrl(orderRepo, sampleCtrl);

    g_prodCtrl = &prodCtrl;
    SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);

    prodCtrl.start();

    // Recover PRODUCING orders persisted from a previous session.
    for (auto& o : orderCtrl.listByStatus(Model::OrderStatus::PRODUCING))
        prodCtrl.enqueue(o);

    View::MainMenuView menu(sampleCtrl, orderCtrl, prodCtrl, shipCtrl);
    menu.run();

    prodCtrl.stop();
    return 0;
}
