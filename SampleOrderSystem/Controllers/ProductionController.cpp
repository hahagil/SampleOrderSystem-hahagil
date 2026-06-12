#include "ProductionController.h"
#include <cmath>
#include <chrono>

namespace Controller {

ProductionController::ProductionController(
    Repository::IRepository<Model::Order>&  orderRepo,
    Repository::IRepository<Model::Sample>& sampleRepo)
    : m_orderRepo(orderRepo), m_sampleRepo(sampleRepo) {}

ProductionController::~ProductionController() {
    stop();
}

void ProductionController::start() {
    m_stopFlag = false;
    m_worker = std::thread(&ProductionController::workerLoop, this);
}

void ProductionController::stop() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stopFlag = true;
    }
    m_cv.notify_all();
    if (m_worker.joinable()) m_worker.join();
}

void ProductionController::enqueue(const Model::Order& order) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(order);
    }
    m_cv.notify_one();
}

std::optional<Model::Order> ProductionController::currentOrder() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_current;
}

std::vector<Model::Order> ProductionController::queueSnapshot() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<Model::Order> result;
    auto q = m_queue;
    while (!q.empty()) { result.push_back(q.front()); q.pop(); }
    return result;
}

int ProductionController::producedCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_producedCount;
}

int ProductionController::calcProductionQty(int qty, double yield) {
    return static_cast<int>(std::ceil(qty / (yield * 0.9)));
}

void ProductionController::workerLoop() {
    while (true) {
        Model::Order order;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [&]{ return !m_queue.empty() || m_stopFlag; });
            if (m_stopFlag && m_queue.empty()) return;
            order = m_queue.front();
            m_queue.pop();
            m_current = order;
            m_producedCount = 0;
        }

        auto sampleOpt = m_sampleRepo.findById(order.sampleId);
        if (!sampleOpt) continue;

        int totalQty = calcProductionQty(order.quantity, sampleOpt->yield);
        auto tick = std::chrono::minutes(sampleOpt->unitProductionTime);

        bool aborted = false;
        for (int i = 0; i < totalQty; ++i) {
            std::unique_lock<std::mutex> lock(m_mutex);
            bool stopped = m_cv.wait_for(lock, tick, [&]{ return m_stopFlag; });
            if (stopped) { aborted = true; break; }
            ++m_producedCount;
        }

        if (!aborted) {
            // New items arrive in stock; order quantity will be allocated at shipment
            auto sample = m_sampleRepo.findById(order.sampleId);
            if (sample) {
                sample->stock += totalQty;
                m_sampleRepo.update(*sample);
            }
            order.status = Model::OrderStatus::CONFIRMED;
            m_orderRepo.update(order);
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_current = std::nullopt;
        }
    }
}

} // namespace Controller
