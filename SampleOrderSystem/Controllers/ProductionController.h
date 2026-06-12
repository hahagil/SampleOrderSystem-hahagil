#pragma once
#include "../Repositories/IRepository.h"
#include "../Models/Order.h"
#include "../Models/Sample.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <optional>
#include <vector>

namespace Controller {

class ProductionController {
public:
    ProductionController(Repository::IRepository<Model::Order>&  orderRepo,
                         Repository::IRepository<Model::Sample>& sampleRepo);
    ~ProductionController();

    void start();
    void stop();

    void enqueue(const Model::Order& order);
    std::optional<Model::Order> currentOrder() const;
    std::vector<Model::Order>   queueSnapshot() const;
    int                         producedCount() const;

    static int calcProductionQty(int qty, double yield);

private:
    void workerLoop();

    Repository::IRepository<Model::Order>&  m_orderRepo;
    Repository::IRepository<Model::Sample>& m_sampleRepo;

    std::queue<Model::Order>     m_queue;
    mutable std::mutex           m_mutex;
    std::condition_variable      m_cv;
    bool                         m_stopFlag = false;
    std::thread                  m_worker;
    std::optional<Model::Order>  m_current;
    int                          m_producedCount = 0;
};

} // namespace Controller
