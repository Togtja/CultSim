#include "lua_system.h"
#include "debug/auto_timer.h"

#include <string>

#include <sol/function.hpp>

namespace cs::system
{
LuaSystem::LuaSystem(SystemContext context, sol::table data) : ISystem(context), m_data(data)
{
}

void LuaSystem::initialize()
{
    m_data.get<sol::function>("initialize")();
}

void LuaSystem::deinitialize()
{
    m_data.get<sol::function>("deinitialize")();
}

void LuaSystem::update(float dt)
{
    // CS_AUTOTIMER_EXPRNAME(m_data[sol::meta_method::to_string]().get<std::string>());
    m_data.get<sol::function>("update")(dt);
}

ISystem* LuaSystem::clone()
{
    return new LuaSystem(m_context, m_data);
}

} // namespace cs::system
