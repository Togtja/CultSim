#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <glm/vec2.hpp>
#include <sol/sol.hpp>

namespace cs::lua
{
struct Scenario
{
    std::string name{};
    std::string description{};
    std::string script_path{};

    /** Scenario Bounds */
    glm::vec2 bounds{};

    /** Initially active systems */
    std::vector<std::string> systems{};

    /** How often this scenario should sample data */
    float sampling_rate = 1.f;

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
