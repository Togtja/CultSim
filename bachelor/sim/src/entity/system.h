#pragma once

#include <entt/entity/registry.hpp>

namespace cs::system
{
class ISystem
{
private:
    entt::registry& m_registry;

public:
    ISystem(entt::registry& registry) : m_registry(registry) {}

    virtual void update(float dt) = 0;
};
}
