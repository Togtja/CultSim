#include <entt/entt.hpp>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

int main()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("Debug from logging.");
    spdlog::info("Info from logging.");
    spdlog::warn("Warn from logging.");
    spdlog::error("Error from logging.");

    sol::state lua{};

    lua.set_function("debug", [](const std::string& str) { spdlog::debug(str); });

    std::string script = "debug(\"Hello logging from Lua\")";
    lua.script(script);

    return 0;
}
