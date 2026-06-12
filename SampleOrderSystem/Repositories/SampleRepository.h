#pragma once
#include "IRepository.h"
#include "../Models/Sample.h"
#include <string>

namespace Repository {

class SampleRepository : public IRepository<Model::Sample> {
public:
    explicit SampleRepository(const std::string& filePath);

    void              add(const Model::Sample& item) override;
    bool              update(const Model::Sample& item) override;
    bool              remove(const std::string& id) override;
    std::optional<Model::Sample> findById(const std::string& id) const override;
    std::vector<Model::Sample>   findAll() const override;

private:
    std::string              m_path;
    std::vector<Model::Sample> m_cache;

    void load();
    void save() const;

    static std::string   serialize(const Model::Sample& s);
    static Model::Sample deserialize(const std::string& obj);
};

} // namespace Repository
