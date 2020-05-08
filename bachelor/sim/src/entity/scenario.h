#pragma once

#include "debug/native_collectors.h"

#include <string>
#include <string_view>
#include <vector>

#include <glm/vec2.hpp>
#include <sol/sol.hpp>

namespace cs::lua
{
/**
 * Contains information about a single scenario and the data it needs to run as intended by the developer
 */
struct Scenario
{
    /** Scenario name */
    std::string name{};

    /** A small description of the scenario, shown during sim */
    std::string description{};

    /** The path to the folder used to load the scenario */
    std::string script_path{};

    /** Scenario Bounds */
    glm::vec2 bounds{};

    /** Initially active systems */
    std::vector<sol::object> systems{};

    /** Data collectors from Lua */
    std::vector<debug::LuaCollector> data_collectors{};

    /** How often this scenario should sample data */
    float sampling_rate{1.0f};

    /** Called on startup */
    sol::function init;

    /** Called during update phase per frame */
    sol::function update;

    /** Called during draw phase per frame */
    sol::function draw;

    /** Called once on shutdown */
    sol::function shutdown;
};

/**
 * Load a scenario from file using default parameters
 *
 * @param lua The lua state to load with
 * @param rpath The relative file path to the scenario definition
 * @return The loaded scenario
 */
Scenario quick_load_scenario(sol::state_view lua, std::string_view rpath);
} // namespace cs::lua
