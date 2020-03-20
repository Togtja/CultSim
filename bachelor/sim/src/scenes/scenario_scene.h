#pragma once
#include "entity/scenario.h"
#include "entity/systems/system.h"
#include "debug/data_collector.h"
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
struct Preference;

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

    /** Create a data collector for this scenario */
    debug::DataCollector m_data_collector{};

    /** Keep track of screen resolution */
    glm::vec2 m_resolution{};

    /** Time sim has been running */
    float m_simtime = 0.f;

    /** Current time scale of simulation */
    float m_timescale = 1.f;

public:
    explicit ScenarioScene(std::string_view scenario);
    explicit ScenarioScene(lua::Scenario scenario);

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

    void initialize_simulation();

    void clean_simulation();

    void reset_simulation();

private:
    void bind_actions_for_scene();

    void bind_scenario_lua_functions();

    void setup_docking_ui();

    void draw_scenario_information_ui();

    void draw_time_control_ui();

    void draw_selected_entity_information_ui();

    void update_entity_hover();

    void handle_preference_changed(const Preference& before, const Preference& after);
};
} // namespace cs
