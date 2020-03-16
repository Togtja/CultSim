#pragma once
#include "entity/scenario.h"
#include "random_engine.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

namespace cs::system
{
/**
 * Context used to provide systems with potentially necessary variables
 */
struct SystemContext
{
    entt::registry* registry{nullptr};
    entt::dispatcher* dispatcher{nullptr};
    RandomEngine* rng{nullptr};
    lua::Scenario* scenario{nullptr};
};

class ISystem
{
protected:
    SystemContext m_context;

public:
    ISystem(SystemContext context) : m_context(context)
    {
    }
    virtual ~ISystem() noexcept = default;

    virtual void update(float dt) = 0;
};
} // namespace cs::system
