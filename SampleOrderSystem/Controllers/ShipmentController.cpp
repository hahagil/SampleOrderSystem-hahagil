#include "ShipmentController.h"

namespace Controller {

ShipmentController::ShipmentController(Repository::IRepository<Model::Order>& orderRepo,
                                       SampleController&                      sampleCtrl)
    : m_orderRepo(orderRepo), m_sampleCtrl(sampleCtrl) {}

std::vector<Model::Order> ShipmentController::listConfirmed() const {
    return {}; // stub
}

bool ShipmentController::ship(const std::string& orderId) {
    return false; // stub
}

} // namespace Controller
