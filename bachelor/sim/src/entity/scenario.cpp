#include "scenario.h"
#include "constants.h"
#include "filesystem/filesystem.h"

#include <glm/common.hpp>

#include <spdlog/spdlog.h>

namespace cs::lua
{
Scenario quick_load_scenario(sol::state_view lua, std::string_view rpath)
{
    /** Run new load scenario init */
    const auto& init_file = fs::read_file("script/scenarios/init.lua");
    lua.script(init_file);

    /** Load new scenario values */
    for (const auto& lua_file : fs::list_directory(rpath))
    {
        const auto& full_path = rpath.data() + std::string("/") + lua_file.data();
        spdlog::get("scenario")->debug("loading scenario resource: {}", full_path);

        const auto& data = fs::read_file(full_path);
        lua.script(data);
    }

    /** Get scenario from Lua */
    auto scenario        = lua["scenario"].get<Scenario>();
    scenario.bounds      = glm::ivec2(glm::ceil(scenario.bounds / static_cast<float>(SIM_GRID_SIZE))) * SIM_GRID_SIZE;
    scenario.script_path = rpath;
    lua["scenario"]      = scenario;
    return scenario;
}
} // namespace cs::lua
