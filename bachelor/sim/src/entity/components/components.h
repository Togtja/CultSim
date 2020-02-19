#pragma once

#include "entity/actions/requirement.h"
#include "gfx/render_data.h"
#include "need.h"
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
    glm::vec3 desired_position{};
};

struct Movement
{
    glm::vec2 desired_direction{};
    glm::vec2 direction{};
    float speed = 1.f;
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

    std::vector<ai::Need> pressing_needs{};
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
    bool can_reproduce = false;

    enum ESex
    {
        Male   = true,
        Female = false
    };

    ESex sex = Male;

    uint16_t number_of_children{};
};

struct Produce
{
    enum EProduce
    {
        Food,
        Silk,
        Hemp,
        Drugs,
        Milk
    };

    std::vector<EProduce> produces{};
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

struct Requirement
{
    std::vector<std::unique_ptr<action::IRequirement>> staged_requirements{};

    Requirement()  = default;
    ~Requirement() = default;
    Requirement(const Requirement& other)
    {
        for (const auto& staged_requirement : other.staged_requirements)
        {
            auto requirement = staged_requirement->clone();
            staged_requirements.emplace_back(requirement);
        }
    }
    Requirement& operator=(const Requirement& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        staged_requirements.clear();
        for (const auto& staged_requirement : rhs.staged_requirements)
        {
            auto requirement = staged_requirement->clone();
            staged_requirements.emplace_back(requirement);
        }

        return *this;
    }
};

struct LocationRequirement
{
    glm::vec3 desired_position{};
};

struct VisionRequirement
{
    tags::ETag tags{};
};

struct FindRequirement
{
    tags::ETag tags{};
    glm::vec3 desired_position{};
};

struct Tags
{
    tags::ETag tags{};
};

struct AI
{
};
} // namespace cs::component
