#include "raymarching_renderer.h"
#include "constants.h"
#include "glutil.h"

namespace cs::gfx
{
RaymarchingRenderer::RaymarchingRenderer(Camera& camera) : m_camera(camera)
{
    init();
}

RaymarchingRenderer::~RaymarchingRenderer() noexcept
{
    deinit();
}

void RaymarchingRenderer::clear()
{
    m_spheres.clear();
}

void RaymarchingRenderer::display()
{
    glProgramUniform1f(m_shader, 6, m_spheres.size());
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
    glNamedBufferSubData(m_sphere_buffer, 0, size_bytes(m_spheres), m_spheres.data());

    glUseProgram(m_shader);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void RaymarchingRenderer::reload()
{
    deinit();
    init();
}

void RaymarchingRenderer::draw_sphere(const glm::vec3& pos, float r)
{
    m_spheres.push_back({{pos, r}});
}

void RaymarchingRenderer::init()
{
    /** Shader setup */
    m_shader = fcreate_program({{"shader/raymarch.vert", GL_VERTEX_SHADER}, {"shader/raymarch.frag", GL_FRAGMENT_SHADER}});

    /** Sphere Buffer Setup */
    glCreateBuffers(1, &m_sphere_buffer);
    glNamedBufferStorage(m_sphere_buffer, sizeof(SphereShape) * SIM_MAX_AGENTS, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_sphere_buffer);
}

void RaymarchingRenderer::deinit()
{
    glDeleteProgram(m_shader);
    glDeleteBuffers(1, &m_sphere_buffer);
}
} // namespace cs::gfx
