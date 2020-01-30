#include "sprite_renderer.h"
#include "constants.h"
#include "glutil.h"

#include <algorithm>
#include <iterator>

#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace cs
{
namespace gfx
{
SpriteRenderer::SpriteRenderer()
{
    /** Init Shader */
    auto vertex_shader = fcompile_shader("shader/sprite.vert", GL_VERTEX_SHADER);
    auto frag_shader   = fcompile_shader("shader/sprite.frag", GL_FRAGMENT_SHADER);
    m_shader           = create_program({vertex_shader, frag_shader});

    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);

    if (!static_cast<bool>(m_shader))
    {
        spdlog::critical("could not create sprite shader program");
        std::abort();
    }

    /**
     * Create VBOs
     * TODO: Helper Function
     */
    glCreateBuffers(1, &m_vbo);

    /** TODO: Consider changing it */
    const std::vector<SpriteVertex> quad = {{{-0.5f, -0.5f}, {0.f, 0.f}},
                                            {{-0.5f, 0.5f}, {0.f, 1.f}},
                                            {{0.5f, 0.5f}, {1.f, 1.f}},
                                            {{0.5f, -0.5f}, {1.f, 0.f}}};

    const std::vector<uint8_t> indices = {0, 1, 2, 0, 2, 3};

    /** Copy static sprite data to separate vbo */
    std::vector<uint8_t> transfer_data(size_bytes(quad) + size_bytes(indices));
    memcpy(transfer_data.data(), indices.data(), size_bytes(indices));
    memcpy(transfer_data.data() + size_bytes(indices), quad.data(), size_bytes(quad));

    glNamedBufferStorage(m_vbo, size_bytes(quad) + size_bytes(indices), transfer_data.data(), 0);

    /** Create Other VBO */
    glCreateBuffers(1, &m_ivbo);
    glNamedBufferStorage(m_ivbo,
                         sizeof(SpriteInstanceVertex) * SIM_MAX_AGENTS,
                         nullptr,
                         GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

    /** Acquire pointer to GPU memory so we can write sprite instance data to it later */
    m_instance_data = static_cast<SpriteInstanceVertex*>(glMapNamedBuffer(m_ivbo, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));

    /** Create VAO */
    glCreateVertexArrays(1, &m_vao);

    /** Format VAO*/
    glVertexArrayAttribFormat(m_vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(SpriteVertex, pos));
    glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(SpriteVertex, tex_coord));
    glVertexArrayAttribFormat(m_vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(SpriteInstanceVertex, offset));
    glVertexArrayAttribFormat(m_vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof(SpriteInstanceVertex, color));
    glVertexArrayAttribIFormat(m_vao, 4, 1, GL_UNSIGNED_INT, offsetof(SpriteInstanceVertex, texture));

    /** Binding VAO*/
    glVertexArrayAttribBinding(m_vao, 0, 0);
    glVertexArrayAttribBinding(m_vao, 1, 0);
    glVertexArrayAttribBinding(m_vao, 2, 1);
    glVertexArrayAttribBinding(m_vao, 3, 1);
    glVertexArrayAttribBinding(m_vao, 4, 1);

    glVertexArrayBindingDivisor(m_vao, 1, 1);

    glEnableVertexArrayAttrib(m_vao, 0);
    glEnableVertexArrayAttrib(m_vao, 1);
    glEnableVertexArrayAttrib(m_vao, 2);
    glEnableVertexArrayAttrib(m_vao, 3);
    glEnableVertexArrayAttrib(m_vao, 4);

    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, size_bytes(indices), sizeof(SpriteVertex));
    glVertexArrayVertexBuffer(m_vao, 1, m_ivbo, 0, sizeof(SpriteInstanceVertex));

    glVertexArrayElementBuffer(m_vao, m_vbo);
    /** VERTEX FORMAT SETUP */
}

} // namespace gfx

} // namespace cs
