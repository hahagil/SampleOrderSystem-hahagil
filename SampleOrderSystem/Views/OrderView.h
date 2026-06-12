#pragma once
#include "../Controllers/OrderController.h"
#include "../Controllers/SampleController.h"
#include <vector>

namespace View {

class OrderView {
public:
    OrderView(Controller::OrderController&  orderCtrl,
              Controller::SampleController& sampleCtrl);
    void run();

private:
    void create();
    void approveOrReject();
    void listAll();
    void printOrders(const std::vector<Model::Order>& orders) const;

    Controller::OrderController&  orderCtrl_;
    Controller::SampleController& sampleCtrl_;
};

} // namespace View
