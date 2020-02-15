#include "sprite_renderer.h"
#include "constants.h"
#include "glutil.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace cs::gfx
{
SpriteRenderer::SpriteRenderer(Camera& camera) : m_camera(camera)
{
    init_texture_slots();

    /** Initialize Camera */
    m_camera.init(glm::vec3(0.f, 0.f, 27.f));
}

void SpriteRenderer::draw(glm::vec3 pos, glm::vec3 color, SpriteTextureID tex)
{
    // m_instance_data[m_nsprites++] = {pos, color, tex};
}

void SpriteRenderer::display()
{

}

SpriteTextureID SpriteRenderer::get_texture(std::string_view rpath)
{
    auto color_data = load_texture(rpath);
    // auto normal_data = load_texture(...);

    /** Set the texture ID to have appropriate values */
    auto textureID     = m_next_texture_id;
    textureID.length   = 0;
    textureID.index    = 0;
    textureID.flag_lit = 1;

    /** Copy to texture */

    increment_next_texture_id();
    return textureID;
}

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

void SpriteRenderer::init_texture_slots()
{
}
} // namespace cs::gfx
