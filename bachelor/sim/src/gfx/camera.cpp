#include "camera.h"
#include <iostream>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>
#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>

namespace cs
{
namespace gfx
{
void Camera::init(glm::vec3 position)
{
    m_pos = position;
}

glm::mat4 Camera::get_view_matrix() const
{
    auto view_mat   = glm::rotate(glm::mat4(1.f), glm::radians(90.f), {1.f, 0.f, 0.f});
    view_mat        = glm::translate(view_mat, {-m_pos.x, -m_pos.y, -m_pos.z});
    const auto proj = glm::perspectiveFov(glm::radians(90.f), 16.f, 9.f, 0.01f, 2000.f);

    return proj * view_mat;
}

void Camera::move(glm::vec3 delta)
{
    m_pos.x = std::clamp(m_pos.x + (delta.x * m_speed), 0 - (m_bounds.x / 2), m_bounds.x / 2);
    m_pos.z += std::clamp(m_pos.z + (delta.z * m_speed), 0 - (m_bounds.y / 2), m_bounds.y / 2);
}
void Camera::zoom(float zoom)
{
    m_pos.y = std::clamp(m_pos.y + (zoom * m_speed), 1.0f, 50.f);
}

void Camera::set_speed(float speed)
{
    m_speed = std::clamp(speed, 0.0001f, 0.1f);
}

void Camera::set_boundaries(glm::vec2 bounds)
{
    m_bounds = bounds;
}

} // namespace gfx
} // namespace cs
