#include "SampleController.h"

namespace Controller {

SampleController::SampleController(Repository::IRepository<Model::Sample>& repo)
    : m_repo(repo) {}

bool SampleController::addSample(const Model::Sample& s) {
    m_repo.add(s);
    return true;
}

bool SampleController::updateSample(const Model::Sample& s) {
    return m_repo.update(s);
}

std::optional<Model::Sample> SampleController::findById(const std::string& id) const {
    return m_repo.findById(id);
}

std::vector<Model::Sample> SampleController::listAll() const {
    return m_repo.findAll();
}

bool SampleController::reduceStock(const std::string& sampleId, int qty) {
    auto s = m_repo.findById(sampleId);
    if (!s || s->stock < qty) return false;
    s->stock -= qty;
    return m_repo.update(*s);
}

std::vector<Model::Sample> SampleController::search(const std::string& keyword) const {
    std::vector<Model::Sample> result;
    for (const auto& s : m_repo.findAll())
        if (s.sampleId.find(keyword) != std::string::npos ||
            s.name.find(keyword) != std::string::npos)
            result.push_back(s);
    return result;
}

bool SampleController::addStock(const std::string& sampleId, int qty) {
    auto s = m_repo.findById(sampleId);
    if (!s) return false;
    s->stock += qty;
    return m_repo.update(*s);
}

} // namespace Controller
