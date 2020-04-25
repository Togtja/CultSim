#pragma once

#include "camera.h"
#include "debug_renderer.h"
#include "sprite_renderer.h"
#include "uniform_buffer.h"
#include "render_data.h"

namespace cs::gfx
{
class Renderer
{
private:
    Camera m_camera{};

    DebugRenderer m_debug_renderer;

    SpriteRenderer m_sprite_renderer;

    UniformBuffer<glm::mat4> m_matrix_ubo{};

    UniformBuffer<ProgramInformation> m_programinfo_ubo{};

public:
    friend Renderer& get_renderer();

    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&)      = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    /**
     * Clear the contents of all renderers
     */
    void clear();

    /**
     * Render contents of all renderers to screen and write required uniforms
     *
     * @warning Do not call the display methods of the renderers manually, use this instead
     */
    void display();

    /**
     * Get a handle to the debug renderer
     *
     * @return
     */
    DebugRenderer& debug();
    [[nodiscard]] const DebugRenderer& debug() const;

    /**
     * Get a handle to the sprite renderer
     *
     * @return
     */
    SpriteRenderer& sprite();
    [[nodiscard]] const SpriteRenderer& sprite() const;

    void set_camera_position(const glm::vec3& pos);

    void move_camera(glm::vec3 delta);

    void set_camera_position_2d(glm::vec2 position);
    glm::vec2 get_camera_position2d();

    void set_camera_bounds(glm::vec2 bounds);

    void update_program_info(float runtime, glm::vec2 cursorpos, glm::vec2 resolution);

    /**
     * Convert a screen coordinate to a world position
     *
     * @param screen_pos The cursor position relative to the top left corner of the screen
     * @param desired_z The desired Z level to put the cursor at
     * @return The cursor position in world space
     */
    glm::vec3 screen_to_world_pos(glm::ivec2 screen_pos, glm::vec2 viewport_size, float desired_z = 0.f);

private:
    Renderer();
};

/**
 * Get access to the Renderer instance
 *
 * @return Reference to the global renderer
 */
Renderer& get_renderer();

} // namespace cs::gfx
