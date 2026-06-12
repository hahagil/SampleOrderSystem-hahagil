#include "OrderRepository.h"
#include "JsonUtil.h"
#include <algorithm>

namespace Repository {

OrderRepository::OrderRepository(const std::string& filePath)
    : m_path(filePath)
{
    load();
}

void OrderRepository::add(const Model::Order& item) {
    m_cache.push_back(item);
    save();
}

bool OrderRepository::update(const Model::Order& item) {
    for (auto& o : m_cache) {
        if (o.orderId == item.orderId) {
            o = item;
            save();
            return true;
        }
    }
    return false;
}

bool OrderRepository::remove(const std::string& id) {
    auto it = std::remove_if(m_cache.begin(), m_cache.end(),
        [&](const Model::Order& o) { return o.orderId == id; });
    if (it == m_cache.end()) return false;
    m_cache.erase(it, m_cache.end());
    save();
    return true;
}

std::optional<Model::Order> OrderRepository::findById(const std::string& id) const {
    for (const auto& o : m_cache)
        if (o.orderId == id) return o;
    return std::nullopt;
}

std::vector<Model::Order> OrderRepository::findAll() const {
    return m_cache;
}

std::vector<Model::Order> OrderRepository::findByStatus(Model::OrderStatus status) const {
    std::vector<Model::Order> result;
    for (const auto& o : m_cache)
        if (o.status == status) result.push_back(o);
    return result;
}

void OrderRepository::load() {
    m_cache.clear();
    std::string content = JsonUtil::readFile(m_path);
    if (content.empty()) return;
    std::string arr = JsonUtil::extractArray(content, "orders");
    for (const auto& obj : JsonUtil::splitObjects(arr))
        m_cache.push_back(deserialize(obj));
}

void OrderRepository::save() const {
    std::vector<std::string> items;
    items.reserve(m_cache.size());
    for (const auto& o : m_cache)
        items.push_back(serialize(o));
    JsonUtil::writeArrayFile(m_path, "orders", items);
}

std::string OrderRepository::serialize(const Model::Order& o) {
    return "{\"orderId\":\"" + JsonUtil::escapeStr(o.orderId) + "\","
           "\"sampleId\":\"" + JsonUtil::escapeStr(o.sampleId) + "\","
           "\"quantity\":" + std::to_string(o.quantity) + ","
           "\"status\":\"" + statusToStr(o.status) + "\","
           "\"createdAt\":" + std::to_string(static_cast<long long>(o.createdAt)) + "}";
}

Model::Order OrderRepository::deserialize(const std::string& obj) {
    Model::Order o;
    o.orderId   = JsonUtil::getString(obj, "orderId");
    o.sampleId  = JsonUtil::getString(obj, "sampleId");
    o.quantity  = JsonUtil::getInt(obj, "quantity");
    o.status    = strToStatus(JsonUtil::getString(obj, "status"));
    o.createdAt = static_cast<std::time_t>(JsonUtil::getInt64(obj, "createdAt"));
    return o;
}

std::string OrderRepository::statusToStr(Model::OrderStatus s) {
    switch (s) {
        case Model::OrderStatus::RESERVED:  return "RESERVED";
        case Model::OrderStatus::PRODUCING: return "PRODUCING";
        case Model::OrderStatus::CONFIRMED: return "CONFIRMED";
        case Model::OrderStatus::RELEASE:   return "RELEASE";
        case Model::OrderStatus::REJECTED:  return "REJECTED";
        default:                            return "UNKNOWN";
    }
}

Model::OrderStatus OrderRepository::strToStatus(const std::string& s) {
    if (s == "PRODUCING") return Model::OrderStatus::PRODUCING;
    if (s == "CONFIRMED") return Model::OrderStatus::CONFIRMED;
    if (s == "RELEASE")   return Model::OrderStatus::RELEASE;
    if (s == "REJECTED")  return Model::OrderStatus::REJECTED;
    return Model::OrderStatus::RESERVED;
}

} // namespace Repository
