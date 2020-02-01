#pragma once

#include "render_data.h"

#include <vector>

namespace cs
{
namespace gfx
{
class SpriteRenderer
{
private:
    uint32_t m_vao{};

    /** Template VBO */
    uint32_t m_vbo{};

    /** Instance VBO */
    uint32_t m_ivbo{};

    uint32_t m_shader{};

    /** Pointer to GPU memory where the instance variables are */
    SpriteInstanceVertex* m_instance_data{};

    /** Current number of sprites about to be drawn */
    uint32_t m_nsprites = 0u;

public:
    SpriteRenderer();

    /**
     * Add a new sprite to the draw list using the given position color and texture
     *
     * @param pos The position to draw the sprite at in world coordinates
     * @param color The color tint to apply to the sprite texture
     * @param tex The texture ID of the texture that you want to draw
     */
    void draw(glm::vec3 pos, glm::vec3 color, SpriteTextureID tex);

    /**
     * Display submits the draw list for this frame to the GPU and renders the scene
     */
    void display();
};

} // namespace gfx
} // namespace cs
