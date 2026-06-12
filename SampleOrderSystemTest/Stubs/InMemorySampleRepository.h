#pragma once
#include "../../SampleOrderSystem/Repositories/IRepository.h"
#include "../../SampleOrderSystem/Models/Sample.h"
#include <vector>
#include <optional>
#include <algorithm>
#include <mutex>

namespace Stub {

class InMemorySampleRepository : public Repository::IRepository<Model::Sample> {
public:
    void add(const Model::Sample& item) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data.push_back(item);
    }

    bool update(const Model::Sample& item) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& s : m_data) {
            if (s.sampleId == item.sampleId) { s = item; return true; }
        }
        return false;
    }

    bool remove(const std::string& id) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = std::remove_if(m_data.begin(), m_data.end(),
            [&](const Model::Sample& s) { return s.sampleId == id; });
        if (it == m_data.end()) return false;
        m_data.erase(it, m_data.end());
        return true;
    }

    std::optional<Model::Sample> findById(const std::string& id) const override {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& s : m_data)
            if (s.sampleId == id) return s;
        return std::nullopt;
    }

    std::vector<Model::Sample> findAll() const override {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data;
    }

private:
    mutable std::mutex         m_mutex;
    std::vector<Model::Sample> m_data;
};

} // namespace Stub
