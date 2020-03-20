#pragma once

#include <cstdint>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace cs::gfx
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

    /** Index into material array to use for this sprite */
    uint16_t material_idx : 3;

    uint8_t bigrees{0};

    uint8_t scale{10};

    operator uint32_t()
    {
        return static_cast<uint32_t>(*this);
    }
};

/**
 * Describes a surface material
 */
struct Material
{
    float diffuse  = 1.f;
    float specular = 0.5f;
    float gloss    = 50.f;
    float emissive = 0.f;
};

/**
 * Describes a sunlight environment
 */
struct Environment
{
    glm::vec4 sun_color{1.f, 1.f, 1.f, 1.f};
    glm::vec4 ambient_color{0.1f, 0.1f, 0.2f, 1.f};
    glm::vec3 sun_direction{1.f, 1.f, 1.f};
    float _padding;
};

/**
 * The vertex layout of a sprite used by the sprite renderer
 */
struct SpriteVertex
{
    glm::vec3 pos{};
    glm::vec2 tex_coord{};
};

/**
 * The vertex layout of an instance of a sprite used by the sprite renderer
 */
struct SpriteInstanceVertex
{
    glm::vec3 offset{};
    glm::vec3 color{};
    SpriteTextureID texture{};
};

/**
 * The vertex layout used by primitive shapes such as grids, cylinders, circles etc
 */
struct PrimitiveVertex
{
    glm::vec3 position{};
    glm::vec3 color{};
};

struct DebugInstanceVertex
{
    glm::vec3 offset{};
    glm::vec3 scale{};
    glm::vec3 color{};
};

} // namespace cs::gfx
