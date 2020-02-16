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
    /** Setup */
    uint32_t next_image{};
    VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, ~0ull, m_aq_sem, VK_NULL_HANDLE, &next_image));
    VK_CHECK(vkResetCommandPool(m_device, m_cmd_pool, 0u));

    auto cbuf = vk::begin_one_time_cmd_buffer(m_device, m_cmd_pool);

    VkClearColorValue color_clear_value = {48.f / 255.f, 10.f / 255.f, 36.f / 255.f, 1.f};
    VkClearValue clear_value{};
    clear_value.color = color_clear_value;

    VkRenderPassBeginInfo rpass_begin_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpass_begin_info.renderPass            = m_renderpass;
    rpass_begin_info.framebuffer           = m_framebuffers[next_image];
    rpass_begin_info.renderArea            = {0, 0, 1280, 720};
    rpass_begin_info.pClearValues          = &clear_value;
    rpass_begin_info.clearValueCount       = 1;

    /** Draw calls and commands */
    vkCmdBeginRenderPass(cbuf, &rpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdEndRenderPass(cbuf);

    vk::end_one_time_cmd_buffer(cbuf);

    /** Submit work and present */
    VkPipelineStageFlags wait_flags[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submit_info         = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount   = 1;
    submit_info.pCommandBuffers      = &cbuf;
    submit_info.pWaitDstStageMask    = wait_flags;
    submit_info.pWaitSemaphores      = &m_aq_sem;
    submit_info.waitSemaphoreCount   = 1;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores    = &m_rel_sem;

    vkQueueSubmit(m_gfx_queue, 1, &submit_info, VK_NULL_HANDLE);

    VkPresentInfoKHR present_info   = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.pWaitSemaphores    = &m_rel_sem;
    present_info.waitSemaphoreCount = 1;
    present_info.pSwapchains        = &m_swapchain;
    present_info.swapchainCount     = 1;
    present_info.pImageIndices      = &next_image;

    VK_CHECK(vkQueuePresentKHR(m_gfx_queue, &present_info));
    VK_CHECK(vkDeviceWaitIdle(m_device));
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
    m_device    = volkGetLoadedDevice();
    m_swapchain = create_info.swapchain;
    m_gfx_queue = create_info.gfx_queue;

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
