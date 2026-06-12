#pragma once
#include "../Repositories/IRepository.h"
#include "../Models/Sample.h"
#include <string>
#include <vector>
#include <optional>

namespace Controller {

class SampleController {
public:
    explicit SampleController(Repository::IRepository<Model::Sample>& repo);

    bool                          addSample(const Model::Sample& s);
    bool                          updateSample(const Model::Sample& s);
    std::optional<Model::Sample>  findById(const std::string& id) const;
    std::vector<Model::Sample>    listAll() const;
    bool                          reduceStock(const std::string& sampleId, int qty);
    bool                          addStock(const std::string& sampleId, int qty);

private:
    Repository::IRepository<Model::Sample>& m_repo;
};

} // namespace Controller
