#include "sprite_renderer.h"
#include "constants.h"
#include "glutil.h"
#include "vao_builder.h"

#include <algorithm>
#include <iterator>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace cs::gfx
{
SpriteRenderer::SpriteRenderer(Camera& camera) : m_camera(camera)
{
    /** Init Shader */
    auto vertex_shader = fcompile_shader("shader/sprite.vert", GL_VERTEX_SHADER);
    auto frag_shader   = fcompile_shader("shader/sprite.frag", GL_FRAGMENT_SHADER);
    m_shader           = create_program({vertex_shader, frag_shader});

    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

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
    const std::vector<SpriteVertex> quad = {{{-0.5f, 0.f, -0.5f}, {0.f, 0.f}},
                                            {{-0.5f, 0.f, 0.5f}, {0.f, 1.f}},
                                            {{0.5f, 0.f, 0.5f}, {1.f, 1.f}},
                                            {{0.5f, 0.f, -0.5f}, {1.f, 0.f}}};

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
    m_instance_data = static_cast<SpriteInstanceVertex*>(
        glMapNamedBufferRange(m_ivbo,
                              0,
                              sizeof(SpriteInstanceVertex) * SIM_MAX_AGENTS,
                              GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_PERSISTENT_BIT));

    /** Create VAO */
    m_vao = VaoBuilder()
                .attribute(0, 0, 3, GL_FLOAT, offsetof(SpriteVertex, pos))
                .attribute(1, 0, 2, GL_FLOAT, offsetof(SpriteVertex, tex_coord))
                .attribute(2, 1, 3, GL_FLOAT, offsetof(SpriteInstanceVertex, offset))
                .attribute(3, 1, 3, GL_FLOAT, offsetof(SpriteInstanceVertex, color))
                .iattribute(4, 1, 1, GL_UNSIGNED_INT, offsetof(SpriteInstanceVertex, texture))
                .divisor(1, 1)
                .ebo(m_vbo)
                .vbo(m_vbo, 0, size_bytes(indices), sizeof(SpriteVertex))
                .vbo(m_ivbo, 1, 0, sizeof(SpriteInstanceVertex))
                .bind()
                .build();

    glUseProgram(m_shader);

    // Initialize Camera
    m_camera.init(glm::vec3(0.f, 27.f, 0.f));
}

void SpriteRenderer::draw(glm::vec3 pos, glm::vec3 color, SpriteTextureID tex)
{
    m_instance_data[m_nsprites++] = {pos, color, tex};
}

void SpriteRenderer::display()
{
    glFlushMappedNamedBufferRange(m_ivbo, 0, sizeof(SpriteInstanceVertex) * m_nsprites);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

    glUseProgram(m_shader);
    glBindVertexArray(m_vao);

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(m_camera.get_view_matrix()));
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr, m_nsprites);

    m_nsprites = 0u;
}

SpriteTextureID SpriteRenderer::get_texture(std::string_view rpath)
{
    return {};

bool SpriteRenderer::increment_next_texture_id()
{
    if (m_next_texture_id.start == 31)
    {
        ++m_next_texture_id.bind_slot;
        m_next_texture_id.start = 0;
    }

    if (m_next_texture_id.index > 7)
    {
        spdlog::error("sprite renderer is full on textures!");
        return false;
    }

    return true;
}

} // namespace cs
