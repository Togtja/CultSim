#pragma once

#include <string_view>

#include <entt/entity/registry.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <robin_hood.h>
#include <sol/state_view.hpp>
#include <sol/table.hpp>

namespace cs
{
namespace detail
{
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
bool spawn_name_component(entt::entity e, entt::registry& reg, sol::table table);

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
