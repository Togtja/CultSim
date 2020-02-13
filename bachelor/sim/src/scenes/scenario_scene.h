#pragma once

#include "entity/systems/system.h"
#include "scene.h"

#include <string_view>
#include <vector>

#include <entt/entity/registry.hpp>
#include <entt/process/scheduler.hpp>
#include <entt/signal/dispatcher.hpp>

namespace cs
{
/**
 * Generic Scene for running simulation scenarios
 */
class ScenarioScene : public IScene
{
private:
    /** Entity registry for this scene */
    entt::registry m_registry{};

    /** Scheduler used by tasks spawned in this scene */
    entt::scheduler<float> m_scheduler{};

    /** Event dispatcher used within this scene */
    entt::dispatcher m_dispatcher{};

    /** Enabled Systems for this Scenario */
    std::vector<std::unique_ptr<system::ISystem>> m_active_systems{};

    /** Disabled Systems for this Scenario */
    std::vector<std::unique_ptr<system::ISystem>> m_inactive_systems{};

public:
    explicit ScenarioScene(std::string_view scenario);

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
} // namespace cs
