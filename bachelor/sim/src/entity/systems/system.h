#pragma once
#include "entity/scenario.h"
#include "random_engine.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <taskflow/declarations.hpp>

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
    tf::Executor* executor{nullptr};
    sol::state_view* lua_state{nullptr};
};

/**
 * Interface for all behavior systems in CultSim
 *
 * This interface is used for systems that add behavior to components. The system part of an ECS. They provide a convenient way to
 * separate systems in smaller chunks that perform their own initialization and logic. Systems should be independent of each
 * other, but by adding more of them, emergent behavior will naturally appear, which is the power of it all.
 */
class ISystem
{
protected:
    SystemContext m_context;

public:
    ISystem(SystemContext context) : m_context(context)
    {
    }

    ISystem(const ISystem&) = default;
    ISystem& operator=(const ISystem&) = default;

    ISystem(ISystem&&) = default;
    ISystem& operator=(ISystem&&) = default;

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

    /** TODO: Give definition what these two are supposed to do */
    virtual void update(float dt) = 0;

    virtual void update_imgui(){};

    /**
     * Implement in subclasses to create clones of a system
     *
     * @return Pointer to a new heap allocated system
     */
    virtual ISystem* clone() = 0;
};
} // namespace cs::system
