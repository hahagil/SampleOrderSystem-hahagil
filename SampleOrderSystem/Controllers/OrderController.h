#pragma once
#include "../Repositories/IRepository.h"
#include "../Models/Order.h"
#include "../Models/Sample.h"
#include "SampleController.h"
#include "ProductionController.h"
#include <string>
#include <vector>
#include <mutex>

namespace Controller {

class OrderController {
public:
    OrderController(Repository::IRepository<Model::Order>& orderRepo,
                    SampleController&                      sampleCtrl,
                    ProductionController&                  prodCtrl);

    std::string createOrder(const std::string& sampleId, int quantity);
    bool        approve(const std::string& orderId);
    bool        reject(const std::string& orderId);
    std::vector<Model::Order> listByStatus(Model::OrderStatus status) const;
    std::vector<Model::Order> listAll() const;

private:
    Repository::IRepository<Model::Order>& m_orderRepo;
    SampleController&                      m_sampleCtrl;
    ProductionController&                  m_prodCtrl;
    mutable std::mutex                     m_approveMutex;
};

} // namespace Controller
