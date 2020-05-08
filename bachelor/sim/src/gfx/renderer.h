#pragma once

#include "camera.h"
#include "debug_renderer.h"
#include "raymarching_renderer.h"
#include "render_data.h"
#include "sprite_renderer.h"
#include "uniform_buffer.h"

namespace cs::gfx
{
/**
 * Describes what mode we are rendering in
 */
enum class ERenderingMode
{
    Render_2D,
    Render_3D
};

/**
 * The renderer is a Singleton that acts as an accessor and wrapper around the more specialized renderers available in CultSim
 */
class Renderer
{
private:
    /** The active camera for both the 2D and 3D world */
    Camera m_camera{};

    /** Renderer for debug shapes */
    DebugRenderer m_debug_renderer;

    /** Renderer for sprites and 2D */
    SpriteRenderer m_sprite_renderer;

    /** Renderer for 3D world, very primitive */
    RaymarchingRenderer m_raymarch_renderer;

    /** Uniform for view and projection matrices, used by multiple renderers */
    UniformBuffer<MatrixData> m_matrix_ubo{};

    /** Uniform for information about the program, such as resolution and cursor position */
    UniformBuffer<ProgramInformation> m_programinfo_ubo{};

    /** Sunlight environment UBO */
    UniformBuffer<Environment> m_env_ubo{};

    /** Current rendering mode */
    ERenderingMode m_mode = ERenderingMode::Render_2D;

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
    [[nodiscard]] DebugRenderer& debug();
    [[nodiscard]] const DebugRenderer& debug() const;

    /**
     * Get a handle to the sprite renderer
     *
     * @return
     */
    [[nodiscard]] SpriteRenderer& sprite();
    [[nodiscard]] const SpriteRenderer& sprite() const;

    /**
     * Get a handle to the 3d renderer
     *
     * @return
     */
    [[nodiscard]] RaymarchingRenderer& raymarch();
    [[nodiscard]] const RaymarchingRenderer& raymarch() const;

    /**
     * Set the rendering mode
     *
     * @param mode The new rendering mode
     */
    void set_render_mode(ERenderingMode mode);

    /**
     * Set the camera position
     *
     * @param pos A new position in 3D world space
     */
    void set_camera_position(const glm::vec3& pos);

    /**
     * Move the camera by a given delta instantly
     *
     * @param delta How much to move the camera
     */
    void move_camera(glm::vec3 delta);

    /**
     * Set the camera position on the XY plane
     *
     * @param position The new position on the XY plane
     */
    void set_camera_position_2d(glm::vec2 position);

    /**
     * Get the camera position on the XY plane
     *
     * @return The camera position on the XY plane
     */
    glm::vec2 get_camera_position2d();

    /**
     * Set the camera bounds
     *
     * @param bounds The bounds on the XY plane
     */
    void set_camera_bounds(glm::vec2 bounds);

    /**
     * Update the program information so the renderer can use it
     *
     * @param runtime How long since the program or relevant part of the program has started
     * @param cursorpos The current mouse position
     * @param resolution The current screen resolution
     */
    void update_program_info(float runtime, glm::vec2 cursorpos, glm::vec2 resolution);

    /**
     * Set the sun direction in the environment for 2D and 3D
     *
     * @param dir The new sun direction, should be normalized
     */
    void set_sun_direction(glm::vec3 dir);

    /**
     * Set the sun color in the 2D and 3D environment
     *
     * @param col The new environment color
     */
    void set_sun_color(glm::vec4 col);

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
