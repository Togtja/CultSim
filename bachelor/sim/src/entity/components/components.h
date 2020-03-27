#pragma once

#include "entity/memories/container.h"
#include "gfx/render_data.h"
#include "need.h"
#include "random_engine.h"
#include "strategy.h"
#include "tags.h"

#include <vector>

#include <entt/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs::component
{
struct Position
{
    glm::vec3 position{};
};

struct Movement
{
    std::vector<glm::vec3> desired_position{};
    glm::vec2 direction{};
    float speed        = 1.f;
    float avoidance_cd = 0.f;
    int avoid_count{};
};

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
    float time_per_frame = 1.f / 4.f;
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
    using OnCompleteFunction = std::function<void(entt::entity, entt::registry&)>;

    float time_to_complete{};
    float time_spent{};

    int number_of_loops{};
    std::variant<OnCompleteFunction, sol::function> on_complete{};
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

    float max_age{};
};

struct DropItems
{
    enum EDeathDrops
    {
        Meat,
        Wood
    };

    std::vector<EDeathDrops> deathdrops{};
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

struct Delete
{
};

struct Reproduction
{
    enum ESex
    {
        Male   = true,
        Female = false
    };

    ESex sex = Male;
    uint16_t number_of_children{};

    bool lays_eggs = false;
    ESex incubator = Female;

    // Needs age component to work
    float start_fertility{};
    float peak_fertility{};
    float end_fertility{};

    // General Pregnancy stat
    float mean_children_pp = 1.0001f;
    float deviation{};

    float fertility    = -1.f;
    bool has_fertility = false;
};

struct Pregnancy
{
    float time_since_start{};
    float gestation_period{};

    entt::entity father{};

    uint16_t children_in_pregnancy{};
};

struct AI
{
};
} // namespace cs::component
