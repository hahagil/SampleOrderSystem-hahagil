#pragma once
#include "../../SampleOrderSystem/Repositories/IRepository.h"
#include "../../SampleOrderSystem/Models/Order.h"
#include <vector>
#include <optional>
#include <algorithm>

namespace Stub {

class InMemoryOrderRepository : public Repository::IRepository<Model::Order> {
public:
    void add(const Model::Order& item) override {
        m_data.push_back(item);
    }

    bool update(const Model::Order& item) override {
        for (auto& o : m_data) {
            if (o.orderId == item.orderId) { o = item; return true; }
        }
        return false;
    }

    bool remove(const std::string& id) override {
        auto it = std::remove_if(m_data.begin(), m_data.end(),
            [&](const Model::Order& o) { return o.orderId == id; });
        if (it == m_data.end()) return false;
        m_data.erase(it, m_data.end());
        return true;
    }

    std::optional<Model::Order> findById(const std::string& id) const override {
        for (const auto& o : m_data)
            if (o.orderId == id) return o;
        return std::nullopt;
    }

    std::vector<Model::Order> findAll() const override { return m_data; }

    std::vector<Model::Order> findByStatus(Model::OrderStatus status) const {
        std::vector<Model::Order> result;
        for (const auto& o : m_data)
            if (o.status == status) result.push_back(o);
        return result;
    }

private:
    std::vector<Model::Order> m_data;
};

} // namespace Stub
