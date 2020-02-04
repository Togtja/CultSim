#include "debug_renderer.h"
#include "vao_builder.h"
#include "render_data.h"

#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace cs
{
namespace gfx
{
DebugRenderer::DebugRenderer(Camera& camera) : m_camera(camera)
{
    m_vao = VaoBuilder().attribute(0, 0, 2, GL_FLOAT, 0u).build();

    glCreateBuffers(1, &m_linevbo);
    glNamedBufferStorage(m_linevbo, sizeof(PrimitiveVertex) * 2u, nullptr, GL_DYNAMIC_STORAGE_BIT);
}

void DebugRenderer::draw_line(glm::vec2 from, glm::vec2 to, glm::vec3 color)
{
    auto data = std::vector<PrimitiveVertex>{{{from.x, 0.f, from.y}, color}, {{to.x, 0.f, to.y}, color}};

    glNamedBufferSubData(m_linevbo, 0, sizeof(PrimitiveVertex) * 2, data.data());
    glBindVertexArray(m_vao);
    glBindVertexBuffer(0, m_linevbo, 0, sizeof(PrimitiveVertex));
    glUseProgram(m_shader);
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(m_camera.get_view_matrix()));
    glUniform3f(1, color.r, color.g, color.b);
    glDrawArrays(GL_LINES, 0, 2);
}
} // namespace gfx
} // namespace cs
