#pragma once

#include <sol/state_view.hpp>

namespace cs::lua
{
/**
 * Bind our component types to Lua so it can read and understand them
 *
 * @param lua The lua state
 */
void bind_components(sol::state_view lua);
}
