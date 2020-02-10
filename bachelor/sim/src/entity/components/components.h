#pragma once

#include "action.h"
#include "gfx/render_data.h"
#include "need.h"

#include <vector>

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

struct Actions
{
    std::vector<ai::Action> actions{};

    std::vector<ai::Action> staged_actions{};
};

struct AI
{
};
} // namespace cs::component
