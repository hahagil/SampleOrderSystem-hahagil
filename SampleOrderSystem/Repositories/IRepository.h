#pragma once
#include <string>
#include <vector>
#include <optional>

namespace Repository {

template<typename T>
class IRepository {
public:
    virtual ~IRepository() = default;
    virtual void              add(const T& item) = 0;
    virtual bool              update(const T& item) = 0;
    virtual bool              remove(const std::string& id) = 0;
    virtual std::optional<T>  findById(const std::string& id) const = 0;
    virtual std::vector<T>    findAll() const = 0;
};

} // namespace Repository
