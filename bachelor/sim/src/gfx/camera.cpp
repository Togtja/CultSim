#include "camera.h"

#include <algorithm>

#include "glm/gtc/matrix_transform.hpp"


namespace cs
{
namespace gfx
{

void Camera::init(glm::vec3 position, glm::vec2 bounds)
{
    m_pos = position;
    m_bounds = bounds;
    m_speed  = 0.001;
}

glm::mat4 Camera::get_view_matrix()
{

    return glm::lookAt((glm::vec3)(m_pos.x,20.f,m_pos.y),(glm::vec3)(m_pos.x,0.f,m_pos.y),(glm::vec3)(0.f,1.f,0.f));
}

void Camera::move(glm::vec3 movement)
{
    m_pos.x = std::clamp(m_pos.x + (movement.x * m_speed),0-(m_bounds.x/2),m_bounds.x/2);
    m_pos.z += std::clamp(m_pos.z + (movement.z * m_speed),0-(m_bounds.y/2),m_bounds.y/2);
}
void Camera::zoom(float zoom)
{
    m_pos.y = std::clamp(+m_pos.y + (zoom * m_speed), 1.0f, 50.f);
}

void Camera::set_speed(float speed)
{
    m_speed = std::clamp(speed, 0.0001f, 0.1f);
}

} // namespace gfx
} // namespace cs
