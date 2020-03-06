#pragma once

#include "gfx/render_data.h"
#include "need.h"
#include "random_engine.h"
#include "strategy.h"
#include "tags.h"
#include "entity/memories/memory_container.h"

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
    std::string description{};
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

struct Needs
{
    std::vector<ai::Need> needs{};
    std::vector<ai::Need> vital_needs{};
    std::vector<ai::Need> leisure_needs{};
};

struct Vision
{
    std::vector<entt::entity> seen{};
    float vision_radius{};
    uint8_t fov{};
};

struct Smell
{
    float smell_radius{};
};

struct Hearing
{
    float sound_radius{};
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
    uint16_t max_children{};
};

struct Timer
{
    float time_to_complete{};
    float time_spent{};

    int number_of_loops{};

    std::function<void(entt::entity, entt::registry&)> on_complete{};
};

struct Health
{
    float hp{};

    float tickdown_rate{};

    ETag need_tags{};
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

struct Strategies
{
    std::vector<ai::Strategy> strategies{};

    std::vector<ai::Strategy> staged_strategies{};
};

struct LocationRequirement
{
    glm::vec3 desired_position{};
};

struct VisionRequirement
{
    ETag tags{};
};

struct FindRequirement
{
    ETag tags{};
    glm::vec3 desired_position{};
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
    uint16_t size{};
    uint16_t max_size{};

    std::vector<entt::entity> contents{};

    ETag tags{};
};

struct Memory
{
    std::vector<MemoryContainer> memory_storage{};
};

struct AI
{
};
} // namespace cs::component
