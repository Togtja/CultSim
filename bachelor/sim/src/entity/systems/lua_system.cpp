#include "lua_system.h"
#include "debug/auto_timer.h"

#include <string>

namespace cs::system
{
LuaSystem::LuaSystem(SystemContext context, sol::table data) : ISystem(context), m_data(data)
{
}

void LuaSystem::initialize()
{
    m_data["initialize"]();
}

void LuaSystem::deinitialize()
{
    m_data["deinitialize"]();
}

void LuaSystem::update(float dt)
{
    CS_AUTOTIMER_EXPRNAME(m_data["name"].get<std::string>());
    m_data["update"](dt);
}

ISystem* LuaSystem::clone()
{
    return new LuaSystem(m_context, m_data);
}

} // namespace cs::system
