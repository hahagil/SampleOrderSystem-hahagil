#include "SampleRepository.h"
#include "JsonUtil.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace Repository {

SampleRepository::SampleRepository(const std::string& filePath)
    : m_path(filePath)
{
    load();
}

void SampleRepository::add(const Model::Sample& item) {
    m_cache.push_back(item);
    save();
}

bool SampleRepository::update(const Model::Sample& item) {
    for (auto& s : m_cache) {
        if (s.sampleId == item.sampleId) {
            s = item;
            save();
            return true;
        }
    }
    return false;
}

bool SampleRepository::remove(const std::string& id) {
    auto it = std::remove_if(m_cache.begin(), m_cache.end(),
        [&](const Model::Sample& s) { return s.sampleId == id; });
    if (it == m_cache.end()) return false;
    m_cache.erase(it, m_cache.end());
    save();
    return true;
}

std::optional<Model::Sample> SampleRepository::findById(const std::string& id) const {
    for (const auto& s : m_cache)
        if (s.sampleId == id) return s;
    return std::nullopt;
}

std::vector<Model::Sample> SampleRepository::findAll() const {
    return m_cache;
}

void SampleRepository::load() {
    m_cache.clear();
    std::string content = JsonUtil::readFile(m_path);
    if (content.empty()) return;
    std::string arr = JsonUtil::extractArray(content, "samples");
    for (const auto& obj : JsonUtil::splitObjects(arr))
        m_cache.push_back(deserialize(obj));
}

void SampleRepository::save() const {
    std::vector<std::string> items;
    items.reserve(m_cache.size());
    for (const auto& s : m_cache)
        items.push_back(serialize(s));
    JsonUtil::writeArrayFile(m_path, "samples", items);
}

std::string SampleRepository::serialize(const Model::Sample& s) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6) << s.yield;
    return "{\"sampleId\":\"" + JsonUtil::escapeStr(s.sampleId) + "\","
           "\"name\":\"" + JsonUtil::escapeStr(s.name) + "\","
           "\"unitProductionTime\":" + std::to_string(s.unitProductionTime) + ","
           "\"yield\":" + ss.str() + ","
           "\"stock\":" + std::to_string(s.stock) + "}";
}

Model::Sample SampleRepository::deserialize(const std::string& obj) {
    Model::Sample s;
    s.sampleId            = JsonUtil::getString(obj, "sampleId");
    s.name                = JsonUtil::getString(obj, "name");
    s.unitProductionTime  = JsonUtil::getInt(obj, "unitProductionTime");
    s.yield               = JsonUtil::getDouble(obj, "yield");
    s.stock               = JsonUtil::getInt(obj, "stock");
    return s;
}

} // namespace Repository
