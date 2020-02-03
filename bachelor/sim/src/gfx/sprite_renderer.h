#pragma once

#include "render_data.h"
#include "camera.h"

#include <vector>

namespace cs
{
namespace gfx
{
class SpriteRenderer
{
private:

    Camera m_camera;

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

    void clear();

    /**
     *
     */
    void draw(glm::vec3 pos, glm::vec3 color, SpriteTextureID tex);

    void display();
};

} // namespace gfx
} // namespace cs
