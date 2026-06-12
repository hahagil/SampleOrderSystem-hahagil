#pragma once
#include "../Controllers/SampleController.h"
#include "../Controllers/OrderController.h"
#include "../Controllers/ProductionController.h"
#include "../Controllers/ShipmentController.h"
#include <string>

namespace View {

class MainMenuView {
public:
    MainMenuView(Controller::SampleController&     sampleCtrl,
                 Controller::OrderController&      orderCtrl,
                 Controller::ProductionController& prodCtrl,
                 Controller::ShipmentController&   shipCtrl);
    void run();

    static void        pause();
    static std::string readLine();

private:
    Controller::SampleController&     sampleCtrl_;
    Controller::OrderController&      orderCtrl_;
    Controller::ProductionController& prodCtrl_;
    Controller::ShipmentController&   shipCtrl_;
};

} // namespace View
