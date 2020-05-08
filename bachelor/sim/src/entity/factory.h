#pragma once

#include "components/components.h"
#include <entt/entity/registry.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <robin_hood.h>
#include <sol/state_view.hpp>
#include <sol/table.hpp>
#include <string_view>

namespace cs
{
namespace detail
{
component::detail::Trait get_trait(sol::table traits);

/**
 * Spawn a component from Lua
 *
 * This chunk of spawner functions are factory functions for individual components that handle reading in the data from Lua and
 * creating the respective C++ types from that data, and assigning it to their respective entities. Used by the spawn_entity
 * function. This doc applies equally to all spawn_*_component functions.
 *
 * @param e The entity to spawn a component for
 * @param reg The registry used for assigning components
 * @param table The lua data table containing information to spawn the particular component
 * @return True if the component was parsed successfully
 */
bool spawn_position_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_movement_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_sprite_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_vision_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_tag_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_need_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_reproduction_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_strategy_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_health_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_attack_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_memory_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_timer_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_age_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_inventory_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_trait_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_relationship_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_name_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_lua_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_action_component(entt::entity e, entt::registry& reg, sol::table table);
bool spawn_goal_component(entt::entity e, entt::registry& reg, sol::table table);
} // namespace detail

/**
 * Spawn a new entity of the given type at a given location
 *
 * @param reg The resgistry in which to create an entity
 * @param lua The lua state to operate on
 * @param entity The name of the entity lua file to run
 * @param position The position to spawn the entity, if it ends up getting a position component
 * @return Handle to spawned entity
 */
entt::entity spawn_entity(entt::registry& reg, sol::state_view lua, std::string_view entity, glm::vec2 position);

/**
 * Spawn a new entity of the given type
 *
 * @param reg The resgistry in which to create an entity
 * @param lua The lua state to operate on
 * @param entity The name of the entity lua file to run
 * @return Handle to spawned entity
 */
entt::entity spawn_entity(entt::registry& reg, sol::state_view lua, std::string_view entity);
}; // namespace cs
