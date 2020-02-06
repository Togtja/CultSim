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
    float happiness;

    /*-100=Livid - 100 = tranquil*/
    float calm;

    /*-100=Unmotivated - 100 = Very motivated*/
    float motivation;
};

struct Status
{
    /*In percentages from 0 - 100 */

    /*0=Dead - 100 = Full health*/
    float health;

    /*0 = No energy - 100 = Full energy*/
    float engergy;

    /*0 = Starving - 100 = Well fed*/
    float nourishment;
};

struct Attributes
{
    /*In percentages from -100 - 100 */

    /*-100 = repulsive - 100 = enticing  */
    float attractiveness;

    /*-100 = terrifying - 100 = soothing*/
    float soothing;
};

} // namespace cs::component
