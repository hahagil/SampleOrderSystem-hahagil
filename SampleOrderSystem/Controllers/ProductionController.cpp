#include "ProductionController.h"
#include <cmath>

namespace Controller {

ProductionController::ProductionController(
    Repository::IRepository<Model::Order>&  orderRepo,
    Repository::IRepository<Model::Sample>& sampleRepo)
    : m_orderRepo(orderRepo), m_sampleRepo(sampleRepo) {}

ProductionController::~ProductionController() {
    stop();
}

void ProductionController::start() {
    // stub
}

void ProductionController::stop() {
    // stub
}

void ProductionController::enqueue(const Model::Order& order) {
    // stub
}

std::optional<Model::Order> ProductionController::currentOrder() const {
    return std::nullopt; // stub
}

std::vector<Model::Order> ProductionController::queueSnapshot() const {
    return {}; // stub
}

int ProductionController::producedCount() const {
    return 0; // stub
}

int ProductionController::calcProductionQty(int qty, double yield) {
    return 0; // stub
}

void ProductionController::workerLoop() {
    // stub
}

} // namespace Controller
