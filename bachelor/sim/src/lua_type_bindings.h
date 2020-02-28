#pragma once

#include <sol/state_view.hpp>

namespace cs::lua
{
/**
 * Bind glm functions to Lua since they are heavily used
 *
 * @param lua The lua state
 */
void bind_glm(sol::state_view lua);

/**
 * Bind our component types to Lua so it can read and understand them
 *
 * @param lua The lua state
 */
void bind_components(sol::state_view lua);

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
} // namespace cs::lua
