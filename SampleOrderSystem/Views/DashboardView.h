#pragma once
#include "../Controllers/SampleController.h"
#include "../Controllers/OrderController.h"

namespace View {

class DashboardView {
public:
    DashboardView(Controller::SampleController& sampleCtrl,
                  Controller::OrderController&  orderCtrl);
    void run();

private:
    Controller::SampleController& sampleCtrl_;
    Controller::OrderController&  orderCtrl_;
};

} // namespace View
