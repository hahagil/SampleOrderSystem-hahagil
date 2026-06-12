#pragma once
#include "../Repositories/IRepository.h"
#include "../Models/Order.h"
#include "SampleController.h"
#include <string>
#include <vector>

namespace Controller {

class ShipmentController {
public:
    ShipmentController(Repository::IRepository<Model::Order>& orderRepo,
                       SampleController&                      sampleCtrl);

    std::vector<Model::Order> listConfirmed() const;
    bool                      ship(const std::string& orderId);

private:
    Repository::IRepository<Model::Order>& m_orderRepo;
    SampleController&                      m_sampleCtrl;
};

} // namespace Controller
