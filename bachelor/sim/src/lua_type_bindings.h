#pragma once

#include <sol/state_view.hpp>

namespace cs::lua
{
/**
 * Bind glm functions to Lua since they are heavily used
 *
 * @param lua The lua state
 */
void bind_dataonly(sol::state_view lua);

/**
 * Bind our component types to Lua so it can read and understand them
 *
 * @param lua The lua state
 */
void bind_components(sol::state_view lua);

/**
 * Bind our input manager to Lua so it can bind key's to action
 *
 * @param lua The lua state
 */
void bind_input(sol::state_view lua);

/**
 * Bind C++ systems to Lua
 *
 * @param lua The lua state
 */
void bind_systems(sol::state_view lua);

/**
 * Bind utility functions and types
 *
 * @param lua The lua state
 */
void bind_utils(sol::state_view lua);

/**
 * Custom exception and error handler for our lua state
 *
 * @param L The lua state
 * @param maybe_exception Container for what could be an exception or just a lua error
 * @param description The description of the error or exception
 * @return int Stack index or error message
 */
int exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description);
} // namespace cs::lua
