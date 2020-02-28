#include "camera.h"
#include <algorithm>
#include <iostream>

#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>
#include <spdlog/spdlog.h>

namespace cs::gfx
{
void Camera::init(glm::vec3 position)
{
    m_pos = position;
}

glm::mat4 Camera::get_view_matrix() const
{
    // const auto view_mat = glm::translate(glm::mat4(1.f), -m_pos);
    // const auto view_mat = glm::lookAt(m_pos, glm::vec3(m_pos.x, m_pos.y, 0.f), glm::vec3(0.f, 1.f, 0.f));
    const auto proj =
        glm::ortho(m_pos.z * -640.f + m_pos.x, m_pos.z * 640.f + m_pos.x, m_pos.z * -360.f + m_pos.y, m_pos.z * 360.f + m_pos.y);

    return proj;
}

void Camera::move(glm::vec3 delta)
{
    m_pos += delta;
    m_pos.x = glm::clamp(m_pos.x, -m_bounds.x / 2.f, m_bounds.x / 2.f);
    m_pos.y = glm::clamp(m_pos.y, -m_bounds.y / 2.f, m_bounds.y / 2.f);
    m_pos.z = glm::clamp(m_pos.z, 0.05f, 5.f);
}

void Camera::zoom(float zoom)
{
    m_pos.z = std::clamp(m_pos.z + (zoom * m_speed), 0.2f, 50.f);
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
