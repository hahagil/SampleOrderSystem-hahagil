#include "OrderController.h"
#include <ctime>
#include <cstdio>

namespace Controller {

OrderController::OrderController(Repository::IRepository<Model::Order>& orderRepo,
                                 SampleController&                      sampleCtrl,
                                 ProductionController&                  prodCtrl)
    : m_orderRepo(orderRepo), m_sampleCtrl(sampleCtrl), m_prodCtrl(prodCtrl) {}

std::string OrderController::createOrder(const std::string& sampleId, int quantity) {
    if (!m_sampleCtrl.findById(sampleId)) return "";
    if (quantity <= 0) return "";

    auto all = m_orderRepo.findAll();
    char buf[16];
    snprintf(buf, sizeof(buf), "ORD-%04d", static_cast<int>(all.size()) + 1);

    Model::Order o;
    o.orderId   = buf;
    o.sampleId  = sampleId;
    o.quantity  = quantity;
    o.status    = Model::OrderStatus::RESERVED;
    o.createdAt = std::time(nullptr);
    m_orderRepo.add(o);
    return o.orderId;
}

bool OrderController::approve(const std::string& orderId) {
    std::lock_guard<std::mutex> lock(m_approveMutex);

    auto o = m_orderRepo.findById(orderId);
    if (!o || o->status != Model::OrderStatus::RESERVED) return false;

    auto s = m_sampleCtrl.findById(o->sampleId);
    if (!s) return false;

    if (s->stock >= o->quantity) {
        m_sampleCtrl.reduceStock(o->sampleId, o->quantity);
        o->status = Model::OrderStatus::CONFIRMED;
    } else {
        o->status = Model::OrderStatus::PRODUCING;
        m_orderRepo.update(*o);
        m_prodCtrl.enqueue(*o);
        return true;
    }

    m_orderRepo.update(*o);
    return true;
}

bool OrderController::reject(const std::string& orderId) {
    auto o = m_orderRepo.findById(orderId);
    if (!o || o->status != Model::OrderStatus::RESERVED) return false;
    o->status = Model::OrderStatus::REJECTED;
    m_orderRepo.update(*o);
    return true;
}

std::vector<Model::Order> OrderController::listByStatus(Model::OrderStatus status) const {
    std::vector<Model::Order> result;
    for (const auto& o : m_orderRepo.findAll())
        if (o.status == status) result.push_back(o);
    return result;
}

std::vector<Model::Order> OrderController::listAll() const {
    return m_orderRepo.findAll();
}

} // namespace Controller
