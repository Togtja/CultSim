#include "raymarching_renderer.h"
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

void RaymarchingRenderer::display()
{
    glUseProgram(m_shader);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void RaymarchingRenderer::reload()
{
    deinit();
    init();
}

void RaymarchingRenderer::init()
{
    auto vertex_shader = fcompile_shader("shader/raymarch.vert", GL_VERTEX_SHADER);
    auto frag_shader   = fcompile_shader("shader/raymarch.frag", GL_FRAGMENT_SHADER);
    m_shader           = create_program({vertex_shader, frag_shader});
    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);
}

void RaymarchingRenderer::deinit()
{
    glDeleteProgram(m_shader);
}
} // namespace cs::gfx
