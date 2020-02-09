#include "renderer.h"

namespace cs::gfx
{
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

void Renderer::set_camera_position(glm::vec3 pos)
{
    /** Todo */
}

void Renderer::move_camera(glm::vec3 delta)
{
    m_camera.move(delta);
}

void Renderer::set_camera_bounds(glm::vec2 bounds)
{
    m_camera.set_boundaries(bounds);
}

Renderer::Renderer() : m_debug_renderer(m_camera), m_sprite_renderer(m_camera)
{
    m_camera.init({0.f, 0.f, 0.f});
}

Renderer& get_renderer()
{
    static Renderer r{};
    return r;
}

} // namespace cs
