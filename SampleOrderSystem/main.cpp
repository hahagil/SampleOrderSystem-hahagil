#include "Repositories/SampleRepository.h"
#include "Repositories/OrderRepository.h"
#include "Controllers/SampleController.h"
#include "Controllers/OrderController.h"
#include "Controllers/ProductionController.h"
#include "Controllers/ShipmentController.h"
#include "Views/MainMenuView.h"

int main() {
    Repository::SampleRepository sampleRepo("samples.json");
    Repository::OrderRepository  orderRepo("orders.json");

    Controller::SampleController     sampleCtrl(sampleRepo);
    Controller::ProductionController prodCtrl(orderRepo, sampleRepo);
    Controller::OrderController      orderCtrl(orderRepo, sampleCtrl, prodCtrl);
    Controller::ShipmentController   shipCtrl(orderRepo, sampleCtrl);

    prodCtrl.start();

    // PRODUCING 상태 주문 복구 (앱 재시작 시)
    for (auto& o : orderCtrl.listByStatus(Model::OrderStatus::PRODUCING))
        prodCtrl.enqueue(o);

    View::MainMenuView menu(sampleCtrl, orderCtrl, prodCtrl, shipCtrl);
    menu.run();

    prodCtrl.stop();
    return 0;
}
