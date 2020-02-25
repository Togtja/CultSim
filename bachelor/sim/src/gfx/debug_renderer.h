#pragma once

#include "camera.h"
#include "render_data.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs::gfx
{
class DebugRenderer
{
private:
    Camera& m_camera;

    uint32_t m_linevbo{};

    uint32_t m_rectvbo{};

    uint32_t m_circlevbo{};

    uint32_t m_vao{};

    uint32_t m_shader{};

    PrimitiveVertex* m_line_data{nullptr};
    uint32_t m_nlines{};

public:
    explicit DebugRenderer(Camera& camera);

    void draw_line(glm::vec3 from, glm::vec3 to, glm::vec3 color);

    void draw_rect(glm::vec2 tl, glm::vec2 size, glm::vec3 color);

    void draw_circle(glm::vec2 pos, float radius, glm::vec3 color);

    void display();
};
} // namespace cs
