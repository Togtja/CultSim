#pragma once

#include <utility>

#include <entt/entity/registry.hpp>

namespace cs::meta
{
/**
 * Reflect pure data types so they are available for later
 */
void reflect_data_types();

/**
 * Reflect the system types for use with spawning from Lua
 */
void reflect_systems();
} // namespace cs::meta
