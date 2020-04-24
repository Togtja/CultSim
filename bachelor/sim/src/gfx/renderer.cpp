#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace cs::gfx
{
void Renderer::clear()
{
    m_sprite_renderer.clear();
    m_debug_renderer.clear();
}

void Renderer::display()
{
    m_matrix_ubo.flush(m_camera.get_view_matrix());
    m_matrix_ubo.bind(0u);
    m_sprite_renderer.display();
    m_debug_renderer.display();
}

DebugRenderer& Renderer::debug()
{
    return m_debug_renderer;
}

const DebugRenderer& Renderer::debug() const
{
    return m_debug_renderer;
}

SpriteRenderer& Renderer::sprite()
{
    return m_sprite_renderer;
}

const SpriteRenderer& Renderer::sprite() const
{
    return m_sprite_renderer;
}

void Renderer::set_camera_position(const glm::vec3& pos)
{
    m_camera.set_position(pos);
}

void Renderer::move_camera(glm::vec3 delta)
{
    m_camera.move(delta);
}

void Renderer::set_camera_position_2d(glm::vec2 position)
{
    m_camera.set_position_2d(position);
}

glm::vec2 Renderer::get_camera_position2d()
{
    return m_camera.get_position();
}

void Renderer::set_camera_bounds(glm::vec2 bounds)
{
    m_camera.set_boundaries(bounds);
}

glm::vec3 Renderer::screen_to_world_pos(glm::ivec2 screen_pos, glm::vec2 viewport_size, float desired_z)
{
    const auto& view_matrix = m_camera.get_view_matrix();
    return glm::unProject(glm::vec3(screen_pos, desired_z), glm::mat4(1.f), view_matrix, glm::vec4(0.f, 0.f, viewport_size));
}

Renderer::Renderer() : m_debug_renderer(m_camera), m_sprite_renderer(m_camera)
{
    m_camera.init({0.f, 0.f, 1.f});
}

Renderer& get_renderer()
{
    static Renderer r{};
    return r;
}

} // namespace cs::gfx
