#include "camera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace cs
{
namespace gfx
{
void Camera::init(glm::vec2 position)
{
    m_pos = position;

}

glm::mat4 Camera::get_view_matrix()
{

    return glm::lookAt((glm::vec3)(m_pos.x,20.f,m_pos.y),(glm::vec3)(m_pos.x,0.f,m_pos.y),(glm::vec3)(0.f,1.f,0.f));
}

void Camera::turn(GLfloat angle)
{
    if (m_rotation + angle > 360.f)
    {
        m_rotation = (m_rotation + angle) - 360.f;
    }
    else if (m_rotation + angle < 0.f)
    {
        m_rotation = (m_rotation + angle) + 360.f;
    }
    else
    {
        m_rotation += angle;
    }
}
void Camera::move(glm::vec2 movement)
{
    m_pos.x += movement.x * m_speed;
    m_pos.y += movement.y * m_speed;
}
void Camera::zoom(float)
{
}
} // namespace gfx
} // namespace cs
