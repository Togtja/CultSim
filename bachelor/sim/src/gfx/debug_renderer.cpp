#include "debug_renderer.h"
#include "constants.h"
#include "glutil.h"
#include "vao_builder.h"

#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace cs::gfx
{
DebugRenderer::DebugRenderer(Camera& camera) : m_camera(camera)
{
}

void DebugRenderer::draw_line(glm::vec3 from, glm::vec3 to, glm::vec3 color)
{
    m_line_data[m_nlines++] = {{from.x, from.y, from.z}, {color.r, color.g, color.b}};
    m_line_data[m_nlines++] = {{to.x, to.y, to.z}, {color.r, color.g, color.b}};
}

void DebugRenderer::display()
{
    glFlushMappedNamedBufferRange(m_linevbo, 0, sizeof(PrimitiveVertex) * m_nlines);

    glBindVertexArray(m_vao);
    glBindVertexBuffer(0, m_linevbo, 0, sizeof(PrimitiveVertex));
    glUseProgram(m_shader);
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(m_camera.get_view_matrix()));
    glDrawArrays(GL_LINES, 0, m_nlines);
    m_nlines = 0u;
}

void DebugRenderer::init()
{
    m_vao = VaoBuilder().attribute(0, 0, 3, GL_FLOAT, 0u).attribute(1, 0, 3, GL_FLOAT, offsetof(PrimitiveVertex, color)).build();

    glCreateBuffers(1, &m_linevbo);
    glNamedBufferStorage(m_linevbo, sizeof(PrimitiveVertex) * DEBUG_MAX_LINES, nullptr, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);
    m_line_data = static_cast<PrimitiveVertex*>(
        glMapNamedBufferRange(m_linevbo,
                              0u,
                              sizeof(PrimitiveVertex) * DEBUG_MAX_LINES,
                              GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));

    auto vs  = fcompile_shader("shader/debug.vert", GL_VERTEX_SHADER);
    auto fs  = fcompile_shader("shader/debug.frag", GL_FRAGMENT_SHADER);
    m_shader = create_program({vs, fs});
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void DebugRenderer::deinit()
{
    glUnmapNamedBuffer(m_linevbo);
    glDeleteBuffers(1, &m_linevbo);
    glDeleteProgram(m_shader);
    glDeleteVertexArrays(1, &m_vao);
}
} // namespace cs::gfx
