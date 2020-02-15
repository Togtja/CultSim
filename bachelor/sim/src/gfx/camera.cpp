#include "camera.h"
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

namespace cs::gfx
{
void Camera::init(glm::vec3 position)
{
    m_pos = position;
}

glm::mat4 Camera::get_view_matrix() const
{
    const auto view_mat = glm::translate(glm::mat4(1.f), -m_pos);
    const auto proj     = glm::perspectiveFov(glm::radians(90.f), 16.f, 9.f, 1.f, 200.f);

    return proj * view_mat;
}

void Camera::move(glm::vec3 delta)
{
    m_pos += delta;
    m_pos.x = glm::clamp(m_pos.x, -m_bounds.x / 2.f, m_bounds.x / 2.f);
    m_pos.y = glm::clamp(m_pos.y, -m_bounds.y / 2.f, m_bounds.y / 2.f);
    m_pos.z = glm::clamp(m_pos.z, 5.f, 1000.f);
}
void Camera::zoom(float zoom)
{
    m_pos.z = std::clamp(m_pos.y + (zoom * m_speed), 1.0f, 50.f);
}

void Camera::set_speed(float speed)
{
    m_speed = std::clamp(speed, 0.0001f, 0.1f);
}

void Camera::set_boundaries(glm::vec2 bounds)
{
    m_bounds = bounds;
}

} // namespace cs::gfx
