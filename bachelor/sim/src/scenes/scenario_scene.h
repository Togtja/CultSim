#pragma once
#include "common_helpers.h"
#include "debug/data_collector.h"
#include "entity/name_generator.h"
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
#include <robin_hood.h>
#include <taskflow/core/executor.hpp>

namespace cs
{
struct Preference;

/**
 * Generic Scene for running simulation scenarios
 */
class ScenarioScene : public IScene
{
private:
    struct LuaEventHandle
    {
        sol::function func{};
        entt::connection connection{};
    };

    using LuaEventMap = robin_hood::unordered_map<std::string, entt::connection (*)(entt::dispatcher& disp, sol::function& func)>;

    /** Entity registry for this scene */
    entt::registry m_registry{};

    /** Scheduler used by tasks spawned in this scene */
    entt::scheduler<float> m_scheduler{};

    /** Event dispatcher used within this scene */
    entt::dispatcher m_dispatcher{};

    /** Executor for multi threaded sub-operations */
    tf::Executor m_mt_executor{};

    /** Random number generator for this scene */
    RandomEngine m_rng{};

    /** Enabled Systems for this Scenario */
    std::vector<std::unique_ptr<system::ISystem>> m_active_systems{};

    /** Systems that must run in the draw phase for this Scenario */
    std::vector<std::unique_ptr<system::ISystem>> m_draw_systems{};

    /** Storage of bound lua events */
    std::vector<std::unique_ptr<LuaEventHandle>> m_lua_event_handlers{};

    /** Notifications */
    std::vector<Notification> m_notifications{};

    /** Storage of possible events for lua to subscribe to */
    LuaEventMap m_lua_ebinder{};

    /** The scenario we are running */
    lua::Scenario m_scenario;

    /** Create a data collector for this scenario */
    debug::DataCollector m_data_collector{};

    /** Name generator used for scenario */
    NameGenerator m_name_generator{};

    /** Scenario Preferences */
    lua::ScenarioLoadPreferences m_scenario_preferences{};

    /** Keep track of screen resolution */
    glm::vec2 m_resolution{};

    /** Time sim has been running */
    double m_simtime{0.0};

    /** Current time scale of simulation */
    int m_timescale{1};

    /** If we are currently paused or not*/
    bool m_paused{false};

public:
    explicit ScenarioScene(std::string_view scenario, const lua::ScenarioLoadPreferences& preferences);

    /** Scene Interface */
    void on_enter() override;

    /** Scene Interface */
    void on_exit() override;

    /** Scene Interface */
    bool update(float dt) override;

    /** Scene Interface */
    bool draw() override;

    /** Load all variables and data about the current scenario */
    void initialize_simulation();

    /** Clean up and reset all variables and the data related to the ongoing simulation */
    void clean_simulation();

    /** Utility to clean an initialize simulation */
    void reset_simulation();

private:
    /** Set up input bindings for this scene */
    void bind_actions_for_scene();

    /** Bind available lua event subscriptions to the lua state */
    void bind_available_lua_events();

    /** Bind scene specific Lua functions to the lua state */
    void bind_scenario_lua_functions();

    /** Set up docking with ImGui so we can dock the UI to the entire screen */
    void setup_docking_ui();

    /** Draw information about the scenario in the main UI */
    void draw_scenario_information_ui();

    /** Draw any active notifications */
    void draw_notifications(float dt);

    /** Draw the time control UI */
    void draw_time_control_ui();

    /** Draw information about the selected entity */
    void draw_selected_entity_information_ui();

    /** Update the state of currently hovered entity */
    void update_entity_hover();

    /** Handles preference change events, specifically for resolution changes */
    void handle_preference_changed(const Preference& before, const Preference& after);
};

/**
 * Wraps a sol function call for use with an event dispatcher
 *
 * @tparam Ev The event type
 * @param func The Lua function to wrap
 * @param event The event to pass to the function as data
 */
template<typename Ev>
void sol_function_wrapper(sol::function& func, const Ev& event)
{
    func(event);
}

/**
 * Function used to bind a Lua function to an event
 *
 * @tparam Ev The type of the event
 * @param disp The event dispatcher to subscribe with
 * @param func The lua function to subscribe
 * @return A connection handle to this event connection so it can be released later
 */
template<typename Ev>
entt::connection lua_binder(entt::dispatcher& disp, sol::function& func)
{
    return disp.sink<Ev>().template connect<&sol_function_wrapper<Ev>>(func);
}

} // namespace cs
