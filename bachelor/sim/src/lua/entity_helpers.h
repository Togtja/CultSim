#pragma once

#include <entt/entity/registry.hpp>

namespace cs::lua
{
class EntityCommand
{
private:
    entt::registry* m_registry{nullptr};

public:
    EntityCommand(entt::registry* registry) : m_registry(registry)
    {
    }

    virtual void execute(entt::entity e) = 0;
};

} // namespace cs::lua
