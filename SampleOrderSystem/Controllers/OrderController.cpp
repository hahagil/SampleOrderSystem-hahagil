#include "OrderController.h"

namespace Controller {

OrderController::OrderController(Repository::IRepository<Model::Order>& orderRepo,
                                 SampleController&                      sampleCtrl,
                                 ProductionController&                  prodCtrl)
    : m_orderRepo(orderRepo), m_sampleCtrl(sampleCtrl), m_prodCtrl(prodCtrl) {}

std::string OrderController::createOrder(const std::string& sampleId, int quantity) {
    return ""; // stub
}

bool OrderController::approve(const std::string& orderId) {
    return false; // stub
}

bool OrderController::reject(const std::string& orderId) {
    return false; // stub
}

std::vector<Model::Order> OrderController::listByStatus(Model::OrderStatus status) const {
    return {}; // stub
}

std::vector<Model::Order> OrderController::listAll() const {
    return {}; // stub
}

} // namespace Controller
