#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs::gfx
{
/**
 * A camera in the 2D or 3D world
 */
class Camera
{
private:
    /** Current camera position */
    glm::vec3 m_pos = {0.f, 0.f, 0.f};

    /** Current bounds that affect this camera */
    glm::vec2 m_bounds = {200.f, 200.f};

    /** Zoom speed of camera */
    float m_zoom_speed = 10.f;

public:
    Camera() = default;

    /**
     * Initialize the Camera with values dependent on scene
     *
     * @param position The Initial position of the camera
     */
    void init(glm::vec3 position);

    /**
     * Get the current view matrix from the camera
     *
     * @return Current view matrix based on the camera state
     */
    [[nodiscard]] glm::mat4 get_view_matrix() const;

    /**
     * Get the current view-projection matrix from the camera
     *
     * @return Current view-projection matrix based on the camera state
     */
    [[nodiscard]] glm::mat4 get_view_projection_matrix() const;

    void set_position_2d(glm::vec2 position);

    void set_position(glm::vec3 position);

    glm::vec3 get_position() const;

    /**
     * Move the camera by an arbitrary amount
     *
     * @param delta How much to move on each axis
     * @note The position of the camera is clamped to not go outside of the bounds defined in Camera.m_bounds
     */
    void move(glm::vec3 delta);

    /**
     * Zoom in or out with the camera
     *
     * @param zoom how much to zoom in or out
     * @note zoom can not zoom out beyond 50 or zoom in beyond 1
     */
    void zoom(float zoom);

    /**
     * Set the boundaries of the current scene
     *
     * @param bounds The boundaries of the scene we are currently in;
     */
    void set_boundaries(glm::vec2 bounds);
};
} // namespace cs::gfx
