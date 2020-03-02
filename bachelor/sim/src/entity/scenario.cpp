#include "scenario.h"
#include "filesystem/filesystem.h"

namespace cs::lua
{
Scenario quick_load_scenario(sol::state_view lua, std::string_view rpath)
{
    const auto& data = fs::read_file(rpath);
    lua.script(data);
    return lua["scenario"].get<Scenario>();
}
} // namespace cs::lua
