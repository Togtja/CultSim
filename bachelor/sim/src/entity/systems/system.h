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

    /**
     * Override if you need to do any system initialization
     *
     * @warning Do <b>NOT</b> override or implement constructors for this
     */
    virtual void initialize(){};

    /**
     * Override if you need to do any system deinitialization
     *
     * @note You usually want to implement this if you implemented initialize
     * @warning Do <b>NOT</b> override or implement destructors for this
     */
    virtual void deinitialize(){};

    virtual void update(float dt) = 0;
};
} // namespace cs::system
