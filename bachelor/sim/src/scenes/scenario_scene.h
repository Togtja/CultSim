#pragma once
#include "entity/scenario.h"
#include "entity/systems/system.h"
#include "debug/data_collector.h"
#include "random_engine.h"
#include "common_helpers.h"
#include "scene.h"

#include <string_view>
#include <vector>

#include <robin_hood.h>
#include <entt/entity/registry.hpp>
#include <entt/meta/factory.hpp>
#include <entt/process/scheduler.hpp>
#include <entt/signal/dispatcher.hpp>
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

    /** Keep track of screen resolution */
    glm::vec2 m_resolution{};

    /** Time sim has been running */
    float m_simtime = 0.f;

    /** Current time scale of simulation */
    int m_timescale = 1;

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

    void bind_available_lua_events();

    void bind_scenario_lua_functions();

    void setup_docking_ui();

    void draw_scenario_information_ui();

    void draw_notifications(float dt);

    void draw_time_control_ui();

    void draw_selected_entity_information_ui();

    void update_entity_hover();

    void handle_preference_changed(const Preference& before, const Preference& after);
};

/**
 * Wraps a sol function call for use with an event dispatcher
 */
template<typename Ev>
void sol_function_wrapper(sol::function& func, const Ev& event)
{
    func(event);
}

/**
 * Function used to bind a Lua function to an event
 */
template<typename Ev>
entt::connection lua_binder(entt::dispatcher& disp, sol::function& func)
{
    return disp.sink<Ev>().template connect<&sol_function_wrapper<Ev>>(func);
}

} // namespace cs
