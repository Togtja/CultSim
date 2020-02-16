#include "sprite_renderer.h"
#include "constants.h"
#include "glutil.h"
#include "vkutil.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace cs::gfx
{
SpriteRenderer::SpriteRenderer(Camera& camera) : m_camera(camera)
{
}

void SpriteRenderer::draw(glm::vec3 pos, glm::vec3 color, SpriteTextureID tex)
{
    // m_instance_data[m_nsprites++] = {pos, color, tex};
}

void SpriteRenderer::display()
{
    auto cbuf = vk::begin_one_time_cmd_buffer(m_device, m_cmd_pool);

    vk::end_one_time_cmd_buffer(cbuf);
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

void SpriteRenderer::init(const SpriteRendererCreateInfo& create_info)
{
    m_device     = volkGetLoadedDevice();
    m_renderpass = vk::create_render_pass(m_device, create_info.sc_format);

    for (auto image : create_info.sc_image_views)
    {
        m_framebuffers.push_back(vk::create_framebuffer(m_device, m_renderpass, image, {1280, 720}));
    }

    m_aq_sem  = vk::create_semaphore(m_device);
    m_rel_sem = vk::create_semaphore(m_device);

    m_cmd_pool = vk::create_command_pool(m_device, create_info.gfx_queue_idx, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

    init_texture_slots();
}

void SpriteRenderer::deinit()
{
    vkDestroyCommandPool(m_device, m_cmd_pool, nullptr);

    vkDestroySemaphore(m_device, m_rel_sem, nullptr);
    vkDestroySemaphore(m_device, m_aq_sem, nullptr);

    for (auto fb : m_framebuffers)
    {
        vkDestroyFramebuffer(m_device, fb, nullptr);
    }

    vkDestroyRenderPass(m_device, m_renderpass, nullptr);
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
