#include "SampleController.h"

namespace Controller {

SampleController::SampleController(Repository::IRepository<Model::Sample>& repo)
    : m_repo(repo) {}

bool SampleController::addSample(const Model::Sample& s) {
    return false; // stub
}

bool SampleController::updateSample(const Model::Sample& s) {
    return false; // stub
}

std::optional<Model::Sample> SampleController::findById(const std::string& id) const {
    return std::nullopt; // stub
}

std::vector<Model::Sample> SampleController::listAll() const {
    return {}; // stub
}

bool SampleController::reduceStock(const std::string& sampleId, int qty) {
    return false; // stub
}

bool SampleController::addStock(const std::string& sampleId, int qty) {
    return false; // stub
}

} // namespace Controller
