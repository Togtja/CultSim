#pragma once

#include "action.h"
#include "action_sequence.h"
#include "entity/memories/container.h"
#include "entity/systems/relationship.h"
#include "gfx/render_data.h"
#include "goal.h"
#include "need.h"
#include "random_engine.h"
#include "strategy.h"
#include "tags.h"

#include <vector>

#include <entt/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <sol/object.hpp>

namespace cs::component
{
/** TODO: Add documentation for each component */

using Delete = entt::tag<"Delete"_hs>;

struct Position
{
    glm::vec3 position{};
};

struct Movement
{
    std::vector<glm::vec3> desired_position{};
    glm::vec2 direction{};
    float speed{1.f};
    float avoidance_cd{};
    int avoid_count{};

    float speed_multi{1.f};
};

/** TODO: Check if this needs renaming */
struct Meta
{
    std::string name{};
};

struct Sprite
{
    gfx::SpriteTextureID texture{};
    glm::vec3 color{};
};

struct Animation
{
    float time_per_frame{1.f / 4.f};
    float current_progress{};
};

struct Need
{
    std::vector<ai::Need> needs{};
    std::vector<ai::Need> vital_needs{};
    std::vector<ai::Need> leisure_needs{};
};

struct Vision
{
    std::vector<entt::entity> seen{};
    float radius{};
    uint8_t fov{};
};

struct Smell
{
    float radius{};
};

struct Hearing
{
    float radius{};
};

struct Timer
{
    /** TODO: Add typedefs for std::functions */
    using OnCompleteFunction = std::variant<std::function<void(entt::entity, entt::registry&)>, sol::function>;

    float time_to_complete{};
    float time_spent{};

    int number_of_loops{};
    OnCompleteFunction on_complete{};
};

struct Health
{
    float health{};
    float tickdown_rate{};

    ETag need_tags{};
};

struct Attack
{
    float damage{};
};

struct Age
{
    float current_age{};

    float average_life_expectancy{};
};

struct Strategy
{
    std::vector<ai::Strategy> strategies{};

    std::vector<ai::Strategy> staged_strategies{};
};

struct LocationRequirement
{
    glm::vec3 desired_position{};

    float closest_distance{};

    float max_time{};

    float elapsed_time{};
};

struct VisionRequirement
{
    ETag tags{};
};

struct FindRequirement
{
    ETag tags{};
    glm::vec3 desired_position{};

    float max_time{};

    float elapsed_time{};
};

struct TagRequirement
{
    ETag tags{};
};

struct InventoryRequirement
{
    int number_of_matching{};

    ETag tags{};
};

struct Tags
{
    ETag tags{};
};

struct Inventory
{
    std::vector<entt::entity> contents{};
    uint16_t size{};
    uint16_t max_size{};
    ETag tags{};
};

struct Memory
{
    int max_memories{};
    float max_retention_time{};

    std::vector<memory::Container> memory_container{};

    Memory(Memory&&) = default;
    Memory& operator=(Memory&&) = default;
    Memory(const Memory&)       = default;
    Memory& operator=(const Memory&) = default;
};

struct Reproduction
{
    enum ESex
    {
        Male   = true,
        Female = false
    };

    ESex sex{Male};
    uint16_t number_of_children{};

    bool lays_eggs{false};
    std::string egg_type;

    ESex incubator{Female};

    /** If true then users says they has specified start/peak/end fertility */
    bool has_fertility{false};

    /** Needs age component to work */
    float start_fertility{};
    float peak_fertility{};
    float end_fertility{};

    /** General Pregnancy stat */
    int mean_children_pp{1};
    float children_deviation{};

    /** Static fertility */
    float fertility{1.f};

    float average_gestation_period{};
    float gestation_deviation{};
};

struct Name
{
    std::string entity_type{};
    std::string name{};
};

/** TODO: Make stuff into struct to differentiate between mother and father */
struct Pregnancy
{
    using ParentsPair = std::pair<entt::entity, entt::entity>;

    float time_since_start{};
    float gestation_period{};

    /** First in the incubator, then second is other parent (In Humans: first is Mom, second is Dad) */
    ParentsPair parents{entt::null, entt::null};

    uint16_t children_in_pregnancy{};

    bool is_egg{false};
};

struct Goal
{
    std::vector<cs::gob::Goal> goals{};
};

struct Action
{
    cs::gob::ActionSequence current_action_sequence{};
    std::vector<cs::gob::ActionSequence> actions{};
};

struct Sphere3D
{
    float radius{1.f};
};

namespace detail
{
/** Defines a single trait */
struct Trait
{
    std::string name;
    std::string desc;

    bool can_inherit{false};
    float inherit_chance{1.0f};

    bool can_mutate{false};
    float mutation_chance{0.0001f};

    sol::function attain;
    sol::function lose;

    sol::function affect;
    sol::function remove_affect;

    bool operator==(Trait trait)
    {
        return name == trait.name;
    }
};
} // namespace detail

/** An Agents traits */
struct Traits
{
    /** The trait a species/aganets start with when running a simulation */
    std::vector<detail::Trait> start_traits;

    /** List of possible atainable traits */
    std::vector<detail::Trait> attainable_traits;

    /** The traits that the agent currently has */
    std::vector<detail::Trait> acquired_traits;
};

struct Relationship
{
    /** Used to add a new entry in the relatip table */
    bool new_create{true};
    uint8_t default_friend{0};
    uint8_t default_romance{0};

    uint8_t self_friend{0};
    uint8_t self_romance{0};

    /** Below used for family tree: */

    /** Mother Ids for Global and Relationship Ids */
    system::ParentEntityIds mom{entt::null, entt::null};

    /** Father Ids for Global and Relationship Ids */
    system::ParentEntityIds dad{entt::null, entt::null};

    /** old_id is entt id for context registry */
    entt::entity old_id{entt::null};
};

/** Component for use in Lua */
template<uint8_t number>
struct LuaComponent
{
    static inline constexpr auto s_lua_comp_id = number;
    sol::object lua_data{};
};

} // namespace cs::component
