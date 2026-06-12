#include "ShipmentController.h"

namespace Controller {

ShipmentController::ShipmentController(Repository::IRepository<Model::Order>& orderRepo,
                                       SampleController&                      sampleCtrl)
    : m_orderRepo(orderRepo), m_sampleCtrl(sampleCtrl) {}

std::vector<Model::Order> ShipmentController::listConfirmed() const {
    std::vector<Model::Order> result;
    for (const auto& o : m_orderRepo.findAll())
        if (o.status == Model::OrderStatus::CONFIRMED) result.push_back(o);
    return result;
}

bool ShipmentController::ship(const std::string& orderId) {
    auto o = m_orderRepo.findById(orderId);
    if (!o || o->status != Model::OrderStatus::CONFIRMED) return false;
    m_sampleCtrl.reduceStock(o->sampleId, o->quantity);
    o->status = Model::OrderStatus::RELEASE;
    m_orderRepo.update(*o);
    return true;
}

} // namespace Controller
