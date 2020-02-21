#include "debug_renderer.h"
#include "glutil.h"
#include "render_data.h"
#include "vao_builder.h"

#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace cs::gfx
{
DebugRenderer::DebugRenderer(Camera& camera) : m_camera(camera)
{
    m_vao = VaoBuilder().attribute(0, 0, 3, GL_FLOAT, 0u).attribute(1, 0, 3, GL_FLOAT, offsetof(PrimitiveVertex, color)).build();

    glCreateBuffers(1, &m_linevbo);
    glNamedBufferStorage(m_linevbo, sizeof(PrimitiveVertex) * 2u, nullptr, GL_DYNAMIC_STORAGE_BIT);

    auto vs  = fcompile_shader("shader/debug.vert", GL_VERTEX_SHADER);
    auto fs  = fcompile_shader("shader/debug.frag", GL_FRAGMENT_SHADER);
    m_shader = create_program({vs, fs});
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void DebugRenderer::draw_line(glm::vec3 from, glm::vec3 to, glm::vec3 color)
{
    auto data = std::vector<PrimitiveVertex>{{{from.x, from.y, from.z}, {1.f, 1.f, 1.f}}, {{to.x, to.y, to.z}, {1.f, 1.f, 1.f}}};

    glNamedBufferSubData(m_linevbo, 0, sizeof(PrimitiveVertex) * 2, data.data());
    glBindVertexArray(m_vao);
    glBindVertexBuffer(0, m_linevbo, 0, sizeof(PrimitiveVertex));
    glUseProgram(m_shader);
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(m_camera.get_view_matrix()));
    glUniform3f(1, color.r, color.g, color.b);
    glDrawArrays(GL_LINES, 0, 2);
}
} // namespace cs::gfx
