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

    /** The debug rendering shader used for all shapes */
    uint32_t m_shader{};

    /** Vertex array object used for all shapes since the layout is the same */
    uint32_t m_vao{};

    /** Rectangles */
    uint32_t m_rectvbo{};
    DebugInstanceVertex* m_rect_data{nullptr};
    uint32_t m_nrects{};
    uint32_t m_rect_buffer_offset{};

    /** Circles */
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

    /** Clear the renderer so it's ready to be filled with new shapes */
    void clear();

    /**
     * Draw a debug line
     *
     * @param from The position to start the line
     * @param to The position to draw the line to
     * @param color The color of the line
     */
    void draw_line(glm::vec3 from, glm::vec3 to, glm::vec3 color);

    /**
     * Draw a debug rectangle
     *
     * @param pos The position to draw the rectangle, centered on the pivot
     * @param color The color of the rectangle
     * @param pivot The pivot point of the rectangle in relative units
     */
    void draw_rect(glm::vec3 pos, const glm::vec2& size, const glm::vec3& color, const glm::vec2& pivot = {0.5f, 0.5f});

    /**
     * Draw a debug circle
     *
     * @param pos The position of the circle
     * @param radius The radius of the circle
     * @param color The color of the circle
     */
    void draw_circle(glm::vec2 pos, float radius, glm::vec3 color);

    /** Show the submitted shapes on screen */
    void display();

private:
    /** Initialize the renderer's data */
    void init();

    /** Initialize the line buffer and data */
    void init_lines();

    /** Initialize the circle buffer and data */
    void init_circles();

    /** Initialize the rectangle buffer and data */
    void init_rects();

    /** Deinitialize the renderer's data */
    void deinit();
};
} // namespace cs::gfx
