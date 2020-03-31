#pragma once

#include "camera.h"
#include "render_data.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs::gfx
{
/**
 * Renderer to aid development that allow developers to draw helper shapes like circles, rectangles and lines
 */
class DebugRenderer
{
private:
    Camera& m_camera;

    uint32_t m_shader{};

    uint32_t m_vao{};

    /** Rectangles */
    uint32_t m_rectvbo{};
    DebugInstanceVertex* m_rect_data{nullptr};
    uint32_t m_nrects{};
    uint32_t m_rect_buffer_offset{};

    /** Rectangles */
    uint32_t m_circlevbo{};
    DebugInstanceVertex* m_circle_data{nullptr};
    uint32_t m_ncircles{};
    uint32_t m_circle_buffer_offset{};

    /** Lines */
    uint32_t m_linevbo{};
    PrimitiveVertex* m_line_data{nullptr};
    uint32_t m_nlines{};

public:
    explicit DebugRenderer(Camera& camera);

    void clear();

    /**
     * Draw a debug line
     *
     * @param from The position to start the line
     * @param to The position to draw the line to
     * @param color The color of the line
     */
    void draw_line(glm::vec3 from, glm::vec3 to, glm::vec3 color);

    void draw_rect(glm::vec3 pos, const glm::vec2& size, const glm::vec3& color, const glm::vec2& pivot = {0.5f, 0.5f});

    void draw_circle(glm::vec2 pos, float radius, glm::vec3 color);

    void display();

    void init();

    void init_lines();

    void init_circles();

    void init_rects();

    void deinit();
};
} // namespace cs::gfx
