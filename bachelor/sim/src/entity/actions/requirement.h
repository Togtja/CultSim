#pragma once
#include <functional>
#include <string>

#include <entt/entity/registry.hpp>

namespace cs::action
{
class IRequirement
{
protected:
    entt::registry& m_registry;
    entt::entity owner{entt::null};

public:
    std::string name{};
    bool predicate = false;

    IRequirement(std::string p_name, entt::registry& registry) : name(p_name), m_registry(registry){};


    virtual IRequirement* clone()
    {
        return new IRequirement(name, m_registry);
    }

    virtual void init(entt::entity entity){};
};
} // namespace cs::action
