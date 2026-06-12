#pragma once
#include "IRepository.h"
#include "../Models/Order.h"
#include <string>
#include <vector>

namespace Repository {

class OrderRepository : public IRepository<Model::Order> {
public:
    explicit OrderRepository(const std::string& filePath);

    void              add(const Model::Order& item) override;
    bool              update(const Model::Order& item) override;
    bool              remove(const std::string& id) override;
    std::optional<Model::Order> findById(const std::string& id) const override;
    std::vector<Model::Order>   findAll() const override;

    std::vector<Model::Order> findByStatus(Model::OrderStatus status) const;

private:
    std::string            m_path;
    std::vector<Model::Order> m_cache;

    void load();
    void save() const;

    static std::string  serialize(const Model::Order& o);
    static Model::Order deserialize(const std::string& obj);

    static std::string        statusToStr(Model::OrderStatus s);
    static Model::OrderStatus strToStatus(const std::string& s);
};

} // namespace Repository
