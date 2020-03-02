#pragma once

#include "entity/scenario.h"
#include "entity/systems/system.h"
#include "random_engine.h"
#include "scene.h"

#include <string_view>
#include <vector>

#include <entt/entity/registry.hpp>
#include <entt/meta/factory.hpp>
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

    /** Random number generator for this scene */
    RandomEngine m_rng{};

    /** Enabled Systems for this Scenario */
    std::vector<entt::meta_any> m_active_systems{};

    /** Disabled Systems for this Scenario */
    std::vector<entt::meta_any> m_inactive_systems{};

    /** The scenario we are running */
    lua::Scenario m_scenario;

public:
    explicit ScenarioScene(std::string_view scenario);
    explicit ScenarioScene(lua::Scenario scenario);

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

private:
    void setup_docking_ui();

    void draw_scenario_information_ui();
};
} // namespace cs
