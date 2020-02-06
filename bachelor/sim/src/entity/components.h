#pragma once

#include "gfx/render_data.h"

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

struct Emotion
{
    /*In percentages from -100 - 100 */

    /*-100=Depressed - 100= Ecstatic*/
    float happiness{};

    /*-100=Livid - 100 = tranquil*/
    float calm{};

    /*-100=Unmotivated - 100 = Very motivated*/
    float motivation{};
};

struct Status
{
    /*In percentages from 0 - 100 */

    /*0=Dead - 100 = Full health*/
    float health = 100;

    /*0 = No energy - 100 = Full energy*/
    float energy = 100;

    /*0 = Starving - 100 = Well fed*/
    float nourishment = 100;
};

struct Attributes
{
    /*In percentages from -100 - 100 */

    /*-100 = repulsive - 100 = enticing  */
    float attractiveness{};

    /*-100 = terrifying - 100 = soothing*/
    float soothing{};
};

struct Sense
{
    enum ESenses
    {
        Vision = 1 << 0,
        Smell  = 1 << 1,
        Sound  = 1 << 2,
        None   = 1 << 4,

    };

    ESenses senses = None;

    float vision_radius{};
    float fov{};

    float smell_radius{};

    float sound_radius{};
};

struct Reproduction
{
    bool can_reproduce= false;

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

struct Drop_Items
{
    enum EDeathDrops
    {
        Meat,
        Wood
    };

    std::vector<EDeathDrops> deathdrops{};
};

struct AI
{
};
} // namespace cs::component
