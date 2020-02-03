#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs
{
namespace gfx
{
class DebugRenderer
{
private:
    uint32_t m_linevbo{};

    uint32_t m_rectvbo{};

    uint32_t m_circlevbo{};

    uint32_t m_vao{};

    uint32_t m_shader{};

public:
    DebugRenderer();

    void draw_line(glm::vec2 from, glm::vec2 to, glm::vec3 color);

    void draw_rect(glm::vec2 tl, glm::vec2 size, glm::vec3 color);

    void draw_circle(glm::vec2 pos, float radius, glm::vec3 color);
};
} // namespace gfx
} // namespace cs
