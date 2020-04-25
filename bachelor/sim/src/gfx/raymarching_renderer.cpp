#include "raymarching_renderer.h"
#include "glutil.h"

namespace cs::gfx
{
RaymarchingRenderer::RaymarchingRenderer(Camera& camera) : m_camera(camera)
{
    /** Init Shader */
    auto vertex_shader = fcompile_shader("shader/raymarch.vert", GL_VERTEX_SHADER);
    auto frag_shader   = fcompile_shader("shader/raymarch.frag", GL_FRAGMENT_SHADER);
    m_shader           = create_program({vertex_shader, frag_shader});
    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);
    glUseProgram(m_shader);
}

RaymarchingRenderer::~RaymarchingRenderer() noexcept
{
    glDeleteProgram(m_shader);
}

void RaymarchingRenderer::display()
{
    glUseProgram(m_shader);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

} // namespace cs::gfx
