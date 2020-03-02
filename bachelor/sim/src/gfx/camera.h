#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

namespace cs::gfx
{
class Camera
{
private:
    glm::vec3 m_pos    = {0.f, 0.f, 0.f};
    glm::vec2 m_bounds = {200.f, 200.f};
    GLfloat m_speed    = 10.f;

public:
    Camera() = default;

    /**
     * Initialize the Camera with values dependent on scene
     *
     * @param position The Initial position of the camera
     */
    void init(glm::vec3 position);

    /**
     * Get the current view-projection matrix from the camera
     *
     * @return Current view-projection matrix based on the camera state
     */
    [[nodiscard]] glm::mat4 get_view_matrix() const;

    void set_position_2d(glm::vec2 position);

    void set_position(glm::vec3 position);

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
     * Set the speed at which the camera moves and zooms
     *
     * @param speed The speed at which we move
     * @note speed is clamped between 0.0001 and 0.1
     */
    void set_speed(float speed);

    /**
     * Set the boundaries of the current scene
     *
     * @param bounds The boundaries of the scene we are currently in;
     */
    void set_boundaries(glm::vec2 bounds);
};
} // namespace cs::gfx
