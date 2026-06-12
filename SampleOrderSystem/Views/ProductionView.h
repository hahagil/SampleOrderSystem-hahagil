#pragma once
#include "../Controllers/ProductionController.h"
#include "../Controllers/SampleController.h"

namespace View {

class ProductionView {
public:
    ProductionView(Controller::ProductionController& prodCtrl,
                   Controller::SampleController&     sampleCtrl);
    void run();

private:
    Controller::ProductionController& prodCtrl_;
    Controller::SampleController&     sampleCtrl_;
};

} // namespace View
