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
/** Tag which tells us that an agent is being deleted soon */
using Delete = entt::tag<"Delete"_hs>;

/** Component which contains the current position of an entity */
struct Position
{
    glm::vec3 position{};
};

/** Component used to move the agent between two positions, as well as ensuring proper collision avoidance. */
struct Movement
{
    /** The list of positions to visit in order to get to a desired position, created via pathfinding */
    std::vector<glm::vec3> desired_position{};

    /** The current direction of movement */
    glm::vec2 direction{};

    float speed{1.f};

    /** How mouch time should pass between attempts to avoid other entities */
    float avoidance_cd{};

    /** Counts how often we have had to avoid things in a row. Used to determine when the path finding should be re-examined */
    int avoid_count{};

    /** A multiplier that is used in addition to the speed of the agent to determine how fast they are moving */
    float speed_multi{1.f};
};

/** TODO: Check if this needs renaming */
/** Component which holds the name of the file containing the information about the entity */
struct Meta
{
    std::string entity_path{};
};

/** Component which holds data needed for rendering a sprite onto a quad */
struct Sprite
{
    gfx::SpriteTextureID texture{};
    glm::vec3 color{};
};

/** Component which holds data required for changing the texture on a quad over time */
struct Animation
{
    /** How much time each frame of animation should be displayed before shifting to the next*/
    float time_per_frame{1.f / 4.f};

    /** Which frame of the animation we are currently on */
    float current_progress{};
};

/** Component which holds data about the entities needs, as well as needs which need working on */
struct Need
{
    /**
     * A list over needs which are ticking down in status and which may be placed into virtual needs should their status drop
     * below a threshhold
     */
    std::vector<ai::Need> needs{};

    /** A list of needs which have dropped below their threshhold and need to be worked on in order to be lowered */
    std::vector<ai::Need> vital_needs{};

    /**
     * A list of needs which do not tick down over time which are selected for being worked on randomly if no vital needs are
     * available
     */
    std::vector<ai::Need> leisure_needs{};
};

/** Component which holds information pertaining to the entities ability to perceive other entities around it */
struct Vision
{
    /** Vector of entities that the entity is able to see currently */
    std::vector<entt::entity> seen{};

    /** How far away an entity is able to see things */
    float radius{};

    /** TODO: Implement this */
    /** The angle at which an entity is able to perceive entities ahead of itself */
    uint8_t fov{};
};

/** Component pertaining to the entities ability to smell other entities around it */
struct Smell
{
    float radius{};
};

/** Component pertaining to the entities ability to hear other entities around it */
struct Hearing
{
    float radius{};
};

/** Component which allows for the implementation of timed events upon an entity*/
struct Timer
{
    using CompleteStdFunction = std::function<void(entt::entity, entt::registry&)>;
    using OnCompleteFunction  = std::variant<CompleteStdFunction, sol::function>;

    float time_to_complete{};
    float time_spent{};

    /**
     * The number of times the timer should run before removing itself from the entity
     *
     * @note A number below 0 indicates that the timer should continue indefinitely
     */
    int number_of_loops{};

    /** A function that will run upon completion of the timer. This is likely user implemented via lua */
    OnCompleteFunction on_complete{};
};

/** Component which keeps track of how healthy an entity is */
struct Health
{
    float health{};

    /** How much health ticks down each second for each need that is unfulfilled */
    float tickdown_rate{};

    /** Each need that has one of the tags described below lowers the health of the entity if it drops to 0 */
    ETag need_tags{};
};

/** Component which keeps track of how much damage an entity does when attacking another */
struct Attack
{
    float damage{};
};

/** Component which keeps track of the age of an entity and how old it should roughly get before dying */
struct Age
{
    float current_age{};

    /** The average age an entity will reach before dying of old age. Deaths of old age will occur somewhere around this number */
    float average_life_expectancy{};
};

/** Component which holds strategies an entity can take, as well as any strategies that are currently staged for execution*/
struct Strategy
{
    /** All strategies available to the entity */
    std::vector<ai::Strategy> strategies{};

    /** Strategies which are currently staged to be used to fullfill one of the entities needs */
    std::vector<ai::Strategy> staged_strategies{};
};

/** Component which holds the position to move to, as well as data required to determine if we cannot reach our destination */
struct LocationRequirement
{
    glm::vec3 desired_position{};

    /**
     * closest_distance keeps track of what the closest we have ever gotten to our destination is. It uses the total length of the
     * path we need to take to get there. And is updated when the remaining distance to the target grows shorter.
     */
    float closest_distance{};

    /**
     * The maximum amount of time to continue trying to move towards the target without closest_distance updating before
     * cancelling the action
     */
    float max_time{};

    /** How much time has passed since the last time closest_distance was updated */
    float elapsed_time{};
};

/** Component which aids in the fulfillment of visionrequirements by holding the tags the target entity needs to have*/
struct VisionRequirement
{
    /** The vision requirement will be completed if we can see an entity that has these tags */
    ETag tags{};
};

/** Component which aids in finding a specific type of entity */
struct FindRequirement
{
    /** The tags that the entity we are looking for has */
    ETag tags{};

    /** The current destination we are moving towards while looking for the target */
    glm::vec3 desired_position{};

    /** The maximum amount of time to spend looking for the entity before canceling the action */
    float max_time{};

    float elapsed_time{};
};

/** Component that holds the tags that a tag requirement is supposed to add to the entity in question */
struct TagRequirement
{
    ETag tags{};
};

/** Component which aids in finding a certain number of entities in the owning entities inventory */
struct InventoryRequirement
{
    int number_of_matching{};

    /** What tags the entities need to have */
    ETag tags{};
};

/** Component which keeps track of the owning entities tags */
struct Tags
{
    ETag tags{};
};

/** Component which keeps track of entities inside the owning entities inventory */
struct Inventory
{
    /** The entities in the inventory */
    std::vector<entt::entity> contents{};

    /** How many entities are in our inventory */
    uint16_t size{};
    uint16_t max_size{};

    /** The combined tags from all entities in the inventory */
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

/** Component which keeps track of the owning entities sexual characteristics and Reproductive activities */
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
    uint16_t mean_children_pp{1};
    float children_deviation{};

    /** Static fertility */
    float fertility{1.f};

    float average_gestation_period{};
    float gestation_deviation{};
};

/** Component which keeps track of an entities type and name */
struct Name
{
    std::string entity_type{};
    std::string name{};
};

/** Component which keeps track of a pregnancy */
struct Pregnancy
{
    /** Struct of who is the parents in a pregnancy*/
    struct Parents
    {
        /** The incubator typically the mom/female */
        entt::entity incubator;

        /** The non-incubator typically the dad/male */
        entt::entity non_incubator;
    };

    /** Time since the start of the pregnancy */
    float time_since_start{};

    /** How long the pregnancy will last */
    float gestation_period{};

    /** First in the incubator, then second is other parent (In Humans: first is Mom, second is Dad) */
    Parents parents{entt::null, entt::null};

    /** The amount of children in the pregnancy (aka twins etc) */
    uint16_t children_in_pregnancy{};

    /** If the pregnancy it self is an egg hatching */
    bool is_egg{false};
};

/** Component which keeps track of all goals an entity has */
struct Goal
{
    std::vector<cs::gob::Goal> goals{};
};

/** Component which keeps track of all possible actions an entity can take, as well as the current action */
struct Action
{
    cs::gob::ActionSequence current_action_sequence{};
    std::vector<cs::gob::ActionSequence> actions{};
};

/** A component holding data necessary for rendering a 3D sphere */
struct Sphere3D
{
    float radius{1.f};
};

namespace detail
{
/** Component which defines a single trait */
struct Trait
{
    /** The name of the trait */
    std::string name;
    /** A short description of the trait */
    std::string desc;

    /**
     * A boolean for whether  the trait can be inheritied by their parents
     * It needs the Reproduction system to work
     */
    bool can_inherit{false};

    /** The probability of inhereting */
    float inherit_chance{1.0f};

    /**
     * Boolean for if you can randomly get this trait when you het born
     * It needs the Reproduction system to work
     */
    bool can_mutate{false};

    /** The probability of randomly getting this trait */
    float mutation_chance{0.0001f};

    /** Lua/user defined function that return a bool for if the agent attained the trait */
    sol::function attain;

    /** Lua/user defined function that return a bool for if the agent lost the trait */
    sol::function lose;

    /** Lua/user defined function that describes how the trait affects the agent */
    sol::function affect;

    /** Lua/user defined function that describes how the trait affects the agent after losing it*/
    sol::function unaffect;

    bool operator==(Trait trait)
    {
        return name == trait.name;
    }
};
} // namespace detail

/** Component keeping track of an Agents traits */
struct Traits
{
    /** The trait a species/aganets start with when running a simulation */
    std::vector<detail::Trait> start_traits;

    /** List of possible atainable traits */
    std::vector<detail::Trait> attainable_traits;

    /** The traits that the agent currently has */
    std::vector<detail::Trait> acquired_traits;
};

/** Component storing information about the relationships between the owning entity and other entities */
struct Relationship
{
    /** Default friends and romance values for strangers */
    uint8_t default_friend{0};
    uint8_t default_romance{0};

    /** Default friends and romance values for them self */
    uint8_t self_friend{0};
    uint8_t self_romance{0};

    /** Below used for family tree: */

    /** Mother Ids for Global and Relationship Ids */
    system::EntityIds mom{entt::null, entt::null};

    /** Father Ids for Global and Relationship Ids */
    system::EntityIds dad{entt::null, entt::null};

    /** My Children Ids for Global and Relationship Ids */
    std::vector<system::EntityIds> children{};

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
