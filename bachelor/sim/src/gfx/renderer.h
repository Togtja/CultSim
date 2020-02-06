#pragma once

#include "camera.h"
#include "sprite_renderer.h"
#include "debug_renderer.h"

namespace cs::gfx
{
class Renderer
{
private:
    Camera m_camera{};

    DebugRenderer m_debug_renderer;

    SpriteRenderer m_sprite_renderer;

public:
    friend Renderer& get_renderer();

    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&)      = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

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

    void set_camera_position(glm::vec3 pos);

    void move_camera(glm::vec3 delta);

    void set_camera_bounds(glm::vec2 bounds);

private:
    Renderer();
};

/**
 * Get access to the Renderer instance
 *
 * @return Reference to the global renderer
 */
Renderer& get_renderer();

} // namespace cs
