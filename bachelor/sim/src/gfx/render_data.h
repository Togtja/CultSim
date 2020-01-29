#pragma once

#include <cstdint>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs
{
namespace gfx
{
/**
 * Describes how to look up what texture to use for a sprite in the rendering system
 */
struct SpriteTextureID
{
    /** What OpenGL Texture binding slot the texture resides at */
    uint16_t bind_slot : 4;

    /** What index in the 2D Texture array at the given binding slot the texture starts at */
    uint16_t start : 5;

    /** The length of the animation associated with this texture */
    uint16_t length : 2;

    /** Current index of the animation */
    uint16_t index : 2;

    /** Should the sprite be affected by lighting */
    uint16_t flag_lit : 1;

    /** Unspecified flag */
    uint16_t flag_b : 1;

    /** Unspecified flag */
    uint16_t flag_c : 1;
};

/**
 * The vertex layout of a sprite used by the sprite renderer
 */
struct SpriteVertex
{
    glm::vec2 pos{};
    glm::vec2 tex_coord{};
};

struct SpriteInstanceVertex
{
    glm::vec3 offset{};
    glm::vec3 color{};
    SpriteTextureID texture{};
};

} // namespace gfx
} // namespace cs
