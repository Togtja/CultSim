
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
    * @Param position The Initial position of the camera
    * @Param bounds The boundaries of the scene
    */
    void init(glm::vec3 position ,glm::vec2 bounds);


    glm::mat4 get_view_matrix();

    /**
    * Move the camera along the (X,Z) plane
    *
    * @Param dir Direction of movement
    *
    * @Note The position of the camera is clamped to not go outside of the bounds defined in Camera.m_bounds
    */
    void move(glm::vec3 dir);

    /**
    * Zoom in or out with the camera
    *
    * @Param zoom how much to zoom in or out
    *
    * @Note zoom can not zoom out beyond 50 or zoom in beyond 1
    */
    void zoom(float zoom);


    /**
    * Set the speed at which the camera moves and zooms
    *
    * @Param speed The speed at which we move
    *
    * @Note speed is clamped between 0.0001 and 0.1
    */
    void set_speed(float speed);
};
} // namespace gfx
} // namespace cs
