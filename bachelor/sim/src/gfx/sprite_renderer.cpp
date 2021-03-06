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

#include <gfx/ImGUI/imgui.h>

namespace cs::gfx
{
SpriteRenderer::SpriteRenderer(Camera& camera) : m_camera(camera)
{
    init_shader();
    init_vbo_and_vao();
    init_texture_slots();
    init_ubos();

    /** Initialize Camera */
    m_camera.init(glm::vec3(0.f, 0.f, 27.f));
}

SpriteRenderer::~SpriteRenderer() noexcept
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ivbo);
    glDeleteProgram(m_shader);

    glDeleteTextures(m_color_texture_handles.size(), m_color_texture_handles.data());
    glDeleteTextures(m_normal_texture_handles.size(), m_normal_texture_handles.data());
}

void SpriteRenderer::clear()
{
    m_nsprites = 0u;
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

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr, m_nsprites);
}

SpriteTextureID SpriteRenderer::get_texture(const std::string& rpath, const std::string& nrpath)
{
    /** If we alreadly loaded this texture, then give it back */
    if (const auto itr = m_texture_cache.find(rpath); itr != m_texture_cache.end())
    {
        spdlog::get("graphics")->trace("used cached texture for {}", rpath);
        return itr->second;
    }

    /** Set the texture ID to have appropriate values */
    auto texture_id         = m_next_texture_id;
    texture_id.length       = 0;
    texture_id.index        = 0;
    texture_id.material_idx = MATERIAL_IDX_DEFAULT;

    /** Load color data */
    const auto color_data = load_texture(rpath);
    glTextureSubImage3D(m_color_texture_handles[texture_id.bind_slot],
                        0,
                        0,
                        0,
                        texture_id.start,
                        TEXTURE_WIDTH,
                        TEXTURE_HEIGHT,
                        1,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        color_data.pixels.data());

    /** Load normal map if provided */
    if (!nrpath.empty())
    {
        spdlog::get("graphics")->trace("loading normal map {}", nrpath);
        const auto normal_data = load_texture(nrpath);
        glTextureSubImage3D(m_normal_texture_handles[texture_id.bind_slot],
                            0,
                            0,
                            0,
                            texture_id.start,
                            TEXTURE_WIDTH,
                            TEXTURE_HEIGHT,
                            1,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE,
                            normal_data.pixels.data());
    }

    increment_next_texture_id();
    m_texture_cache[rpath] = texture_id;
    return texture_id;
}

bool SpriteRenderer::increment_next_texture_id()
{
    if (m_next_texture_id.start++ == 31)
    {
        ++m_next_texture_id.bind_slot;
        m_next_texture_id.start = 0;
    }

    if (m_next_texture_id.index > 7)
    {
        spdlog::get("graphics")->error("sprite renderer is full on textures!");
        return false;
    }

    return true;
}

void SpriteRenderer::init_vbo_and_vao()
{
    glCreateBuffers(1, &m_vbo);

    const std::vector<SpriteVertex> quad = {{{-0.5f, -0.5f, 0.f}, {1.f, 1.f}},
                                            {{-0.5f, 0.5f, 0.f}, {1.f, 0.f}},
                                            {{0.5f, 0.5f, 0.f}, {0.f, 0.f}},
                                            {{0.5f, -0.5f, 0.f}, {0.f, 1.f}}};

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
}

void SpriteRenderer::init_shader()
{
    /** Init Shader */
    m_shader = fcreate_program({{"shader/sprite.vert", GL_VERTEX_SHADER}, {"shader/sprite.frag", GL_FRAGMENT_SHADER}});
}

void SpriteRenderer::init_texture_slots()
{
    constexpr int num_textures = 8;

    /** Create texture bindings for color */
    m_color_texture_handles.resize(num_textures);
    glCreateTextures(GL_TEXTURE_2D_ARRAY, num_textures, m_color_texture_handles.data());
    for (auto tex : m_color_texture_handles)
    {
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTextureStorage3D(tex, 1, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, 32);
    }
    glBindTextures(0, num_textures, m_color_texture_handles.data());

    /** Same for normal textures */
    m_normal_texture_handles.resize(num_textures);
    glCreateTextures(GL_TEXTURE_2D_ARRAY, num_textures, m_normal_texture_handles.data());
    for (auto tex : m_normal_texture_handles)
    {
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        /** The color of a neutral normal map */
        const uint8_t color_data[] = {128, 128, 255, 255};
        glTextureStorage3D(tex, 1, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, 32);
        glClearTexSubImage(tex, 0, 0, 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT, 32, GL_RGBA, GL_UNSIGNED_BYTE, color_data);
    }
    glBindTextures(num_textures, num_textures, m_normal_texture_handles.data());
}

void SpriteRenderer::init_ubos()
{
    m_material_ubo = std::make_unique<UniformBuffer<Material, 8>>();

    /** Set up nospec material */
    auto& mat1    = m_material_ubo->get(MATERIAL_IDX_NOSPEC);
    mat1.specular = 0.f;

    /** Set up shiny material */
    auto& mat2    = m_material_ubo->get(MATERIAL_IDX_SHINY);
    mat2.gloss    = 80.f;
    mat2.specular = 1.25f;

    /** Set up matte material */
    auto& mat3    = m_material_ubo->get(MATERIAL_IDX_MATTE);
    mat3.gloss    = 10.f;
    mat3.specular = 0.7f;

    m_material_ubo->bind(1);
    m_material_ubo->flush();
}

} // namespace cs::gfx
