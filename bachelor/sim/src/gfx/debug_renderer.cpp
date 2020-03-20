#include "debug_renderer.h"
#include "constants.h"
#include "glutil.h"
#include "primitives.h"
#include "vao_builder.h"

#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace cs::gfx
{
DebugRenderer::DebugRenderer(Camera& camera) : m_camera(camera)
{
    init();
}

void DebugRenderer::draw_line(glm::vec3 from, glm::vec3 to, glm::vec3 color)
{
    m_line_data[m_nlines++] = {{from.x, from.y, from.z}, {color.r, color.g, color.b}};
    m_line_data[m_nlines++] = {{to.x, to.y, to.z}, {color.r, color.g, color.b}};
}

void DebugRenderer::draw_rect(glm::vec3 pos, const glm::vec2& size, const glm::vec3& color, const glm::vec2& pivot)
{
    pos -= glm::vec3(size * (pivot - glm::vec2{0.5f, 0.5f}), 0.f);
    m_rect_data[m_nrects++] = {pos, {size, 1.f}, color};
}

void DebugRenderer::draw_circle(glm::vec2 pos, float radius, glm::vec3 color)
{
    m_circle_data[m_ncircles++] = {{pos, 0.f}, {radius, radius, 1.f}, color};
}

void DebugRenderer::display()
{
    glFlushMappedNamedBufferRange(m_linevbo, 0, sizeof(PrimitiveVertex) * m_nlines);
    glFlushMappedNamedBufferRange(m_circlevbo, m_circle_buffer_offset, sizeof(DebugInstanceVertex) * m_ncircles);
    glFlushMappedNamedBufferRange(m_rectvbo, m_rect_buffer_offset, sizeof(DebugInstanceVertex) * m_nrects);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

    glBindVertexArray(m_vao);
    glBindVertexBuffer(0, m_linevbo, 0, sizeof(PrimitiveVertex));
    glUseProgram(m_shader);
    glUniform1f(1, 1.f);

    glDrawArrays(GL_LINES, 0, m_nlines);

    glUniform1f(1, 0.f);
    glVertexArrayElementBuffer(m_vao, m_circlevbo);
    glBindVertexBuffer(0, m_circlevbo, 13 * sizeof(uint32_t), sizeof(PrimitiveVertex));
    glBindVertexBuffer(1, m_circlevbo, m_circle_buffer_offset, sizeof(DebugInstanceVertex));
    glDrawElementsInstanced(GL_LINE_STRIP, 13, GL_UNSIGNED_INT, nullptr, m_ncircles);

    glVertexArrayElementBuffer(m_vao, m_rectvbo);
    glBindVertexBuffer(0, m_rectvbo, 5 * sizeof(uint32_t), sizeof(PrimitiveVertex));
    glBindVertexBuffer(1, m_rectvbo, m_rect_buffer_offset, sizeof(DebugInstanceVertex));
    glDrawElementsInstanced(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, nullptr, m_nrects);

    m_nlines   = 0u;
    m_ncircles = 0u;
    m_nrects   = 0u;
}

void DebugRenderer::init()
{
    m_vao = VaoBuilder()
                .attribute(0, 0, 3, GL_FLOAT, offsetof(PrimitiveVertex, position))
                .attribute(1, 0, 3, GL_FLOAT, offsetof(PrimitiveVertex, color))
                .attribute(2, 1, 3, GL_FLOAT, offsetof(DebugInstanceVertex, offset))
                .attribute(3, 1, 3, GL_FLOAT, offsetof(DebugInstanceVertex, scale))
                .attribute(4, 1, 3, GL_FLOAT, offsetof(DebugInstanceVertex, color))
                .divisor(1, 1)
                .build();

    init_lines();
    init_circles();
    init_rects();

    auto vs  = fcompile_shader("shader/debug.vert", GL_VERTEX_SHADER);
    auto fs  = fcompile_shader("shader/debug.frag", GL_FRAGMENT_SHADER);
    m_shader = create_program({vs, fs});
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void DebugRenderer::init_lines()
{
    glCreateBuffers(1, &m_linevbo);
    glNamedBufferStorage(m_linevbo, sizeof(PrimitiveVertex) * DEBUG_MAX_LINES, nullptr, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);

    m_line_data = static_cast<PrimitiveVertex*>(
        glMapNamedBufferRange(m_linevbo,
                              0u,
                              sizeof(PrimitiveVertex) * DEBUG_MAX_LINES,
                              GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));
}

void DebugRenderer::init_circles()
{
    auto [verts, inds]     = PrimitiveCircle(0.5f, 12).generate_outline();
    m_circle_buffer_offset = size_bytes(verts) + size_bytes(inds);

    glCreateBuffers(1, &m_circlevbo);
    glNamedBufferStorage(m_circlevbo,
                         m_circle_buffer_offset + sizeof(DebugInstanceVertex) * DEBUG_MAX_LINES,
                         nullptr,
                         GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);

    /** Flush the vertex / index data */
    m_circle_data = static_cast<DebugInstanceVertex*>(
        glMapNamedBufferRange(m_circlevbo, 0u, m_circle_buffer_offset, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));

    auto combined_data = combine_buffers(inds, verts);
    memcpy(m_circle_data, combined_data.data(), m_circle_buffer_offset);
    glFlushMappedNamedBufferRange(m_circlevbo, 0u, m_circle_buffer_offset);
    glUnmapNamedBuffer(m_circlevbo);

    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

    /** Map instance portion of the buffer */
    m_circle_data = static_cast<DebugInstanceVertex*>(
        glMapNamedBufferRange(m_circlevbo,
                              m_circle_buffer_offset,
                              sizeof(DebugInstanceVertex) * DEBUG_MAX_LINES,
                              GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));
}

void DebugRenderer::init_rects()
{
    auto [verts, inds]   = PrimitiveQuad(1.f, 1.f).generate_outline();
    m_rect_buffer_offset = size_bytes(verts) + size_bytes(inds);

    glCreateBuffers(1, &m_rectvbo);
    glNamedBufferStorage(m_rectvbo,
                         m_rect_buffer_offset + sizeof(DebugInstanceVertex) * DEBUG_MAX_LINES,
                         nullptr,
                         GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);

    /** Flush the vertex / index data */
    m_rect_data = static_cast<DebugInstanceVertex*>(
        glMapNamedBufferRange(m_rectvbo,
                              0u,
                              m_rect_buffer_offset,
                              GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));

    auto combined_data = combine_buffers(inds, verts);
    memcpy(m_rect_data, combined_data.data(), m_rect_buffer_offset);
    glFlushMappedNamedBufferRange(m_rectvbo, 0u, m_rect_buffer_offset);
    glUnmapNamedBuffer(m_rectvbo);

    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

    /** Map instance portion of the buffer */
    m_rect_data = static_cast<DebugInstanceVertex*>(
        glMapNamedBufferRange(m_rectvbo,
                              m_rect_buffer_offset,
                              sizeof(DebugInstanceVertex) * DEBUG_MAX_LINES,
                              GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));
}

void DebugRenderer::deinit()
{
    glUnmapNamedBuffer(m_rectvbo);
    glDeleteBuffers(1, &m_circlevbo);

    glUnmapNamedBuffer(m_circlevbo);
    glDeleteBuffers(1, &m_rectvbo);

    glUnmapNamedBuffer(m_linevbo);
    glDeleteBuffers(1, &m_linevbo);

    glDeleteProgram(m_shader);
    glDeleteVertexArrays(1, &m_vao);
}
} // namespace cs::gfx
