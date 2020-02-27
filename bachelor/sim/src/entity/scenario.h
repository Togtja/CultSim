#pragma once

#include <string>
#include <vector>

#include <sol/sol.hpp>

namespace cs::lua
{
struct Scenario
{
    std::string name{};
    std::string description{};

    int agent_count{};
    std::vector<std::string> systems{};

    /** Called on startup */
    sol::function init;

    /** Called during update phase per frame */
    sol::function update;

    /** Called during draw phase per frame */
    sol::function draw;

    /** Called once on shutdown */
    sol::function shutdown;
};
} // namespace cs::lua
