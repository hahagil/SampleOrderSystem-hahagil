#pragma once
#include "../Controllers/ShipmentController.h"

namespace View {

class ShipmentView {
public:
    explicit ShipmentView(Controller::ShipmentController& shipCtrl);
    void run();

private:
    Controller::ShipmentController& shipCtrl_;
};

} // namespace View
