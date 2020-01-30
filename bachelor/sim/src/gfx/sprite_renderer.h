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

    SpriteInstanceVertex* m_instance_data{};

public:
    SpriteRenderer();
    /**
     * TODO: this
     *
     */
    void draw(glm::vec3 pos);
};

} // namespace gfx
} // namespace cs
