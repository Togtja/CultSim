#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>

namespace cs
{
namespace gfx
{
class Camera
{
private:
    glm::vec3 m_pos;
    GLfloat m_speed;
    glm::vec2 m_bounds;

public:
    /**
     * Initialize the Camera with values dependent on scene
     *
     * @param position The Initial position of the camera
     */
    void init(glm::vec3 position);

    glm::mat4 get_view_matrix();

    /**
     * Move the camera along the (X,Z) plane
     *
     * @param dir Direction of movement
     * @note The position of the camera is clamped to not go outside of the bounds defined in Camera.m_bounds
     */
    void move(glm::vec3 dir);

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
} // namespace gfx
} // namespace cs
