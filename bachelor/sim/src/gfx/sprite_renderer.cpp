#include "sprite_renderer.h"
#include "constants.h"
#include "glutil.h"
#include "vkutil.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace cs::gfx
{
SpriteRenderer::SpriteRenderer(Camera& camera) : m_camera(camera)
{
}

void SpriteRenderer::draw(glm::vec3 pos, glm::vec3 color, SpriteTextureID tex)
{
    reinterpret_cast<SpriteInstanceVertex*>(m_instance_buffer.data)[m_nsprites++] = {pos, color, tex};
}

void SpriteRenderer::display()
{
    /** Setup */
    uint32_t next_image{};
    VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain->swapchain, ~0ull, m_aq_sem, VK_NULL_HANDLE, &next_image));

    auto result = vkWaitForFences(m_device, 1, &m_fences[next_image], VK_TRUE, 7e+6);
    if (result != VK_SUCCESS)
    {
        return;
    }

    vkResetFences(m_device, 1, &m_fences[next_image]);
    VK_CHECK(vkResetCommandPool(m_device, m_cmd_pools[next_image], 0u));

    auto cbuf = vk::begin_one_time_cmd_buffer(m_device, m_cmd_pools[next_image]);

    VkBufferCopy region{0, 0, sizeof(SpriteInstanceVertex) * m_nsprites};
    vkCmdCopyBuffer(cbuf, m_instance_buffer.buffer, m_instance_frame_buffers[next_image].buffer, 1, &region);

    VkMemoryBarrier memory_barrier = {VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    memory_barrier.dstAccessMask   = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    memory_barrier.srcAccessMask   = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(cbuf,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                         0,
                         1,
                         &memory_barrier,
                         0,
                         nullptr,
                         0,
                         nullptr);

    VkClearColorValue color_clear_value = {48.f / 255.f, 10.f / 255.f, 36.f / 255.f, 1.f};
    VkClearValue clear_value{};
    clear_value.color = color_clear_value;

    VkRenderPassBeginInfo rpass_begin_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpass_begin_info.renderPass            = m_render_pass;
    rpass_begin_info.framebuffer           = m_swapchain->framebuffers[next_image];
    rpass_begin_info.renderArea            = {0, 0, 1280, 720};
    rpass_begin_info.pClearValues          = &clear_value;
    rpass_begin_info.clearValueCount       = 1;

    /** Draw calls and commands */
    vkCmdBeginRenderPass(cbuf, &rpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdPushConstants(cbuf,
                       m_pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(float) * 16,
                       glm::value_ptr(m_camera.get_view_matrix()));

    VkViewport viewport{0,
                        static_cast<float>(m_swapchain->size.height),
                        static_cast<float>(m_swapchain->size.width),
                        -static_cast<float>(m_swapchain->size.height),
                        0,
                        1};
    vkCmdSetViewport(cbuf, 0, 1, &viewport);

    VkRect2D scissor{{0, 0}, m_swapchain->size};
    vkCmdSetScissor(cbuf, 0, 1, &scissor);

    VkDeviceSize offsets[] = {0};

    vkCmdBindPipeline(cbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    vkCmdBindVertexBuffers(cbuf, 0, 1, &m_instance_frame_buffers[next_image].buffer, offsets);
    vkCmdDraw(cbuf, 6, m_nsprites, 0, 0);

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

    vkQueueSubmit(m_gfx_queue, 1, &submit_info, m_fences[next_image]);

    VkPresentInfoKHR present_info   = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.pWaitSemaphores    = &m_rel_sem;
    present_info.waitSemaphoreCount = 1;
    present_info.pSwapchains        = &m_swapchain->swapchain;
    present_info.swapchainCount     = 1;
    present_info.pImageIndices      = &next_image;

    VK_CHECK(vkQueuePresentKHR(m_gfx_queue, &present_info));

    m_nsprites = 0u;
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
    m_device      = volkGetLoadedDevice();
    m_swapchain   = &create_info.swapchain;
    m_render_pass = create_info.render_pass;
    m_gfx_queue   = create_info.gfx_queue;
    m_allocator   = create_info.allocator;

    /** Create synchronization prim's and command pool */
    m_aq_sem  = vk::create_semaphore(m_device);
    m_rel_sem = vk::create_semaphore(m_device);

    for (int i = 0; i < m_swapchain->views.size(); ++i)
    {
        m_cmd_pools.push_back(vk::create_command_pool(m_device, create_info.gfx_queue_idx, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT));
        m_fences.push_back(vk::create_fence(m_device, true));
        m_instance_frame_buffers.push_back(vk::create_buffer(m_allocator,
                                                             sizeof(SpriteInstanceVertex) * SIM_MAX_AGENTS,
                                                             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                             VMA_MEMORY_USAGE_GPU_ONLY));
    }

    init_pipeline();

    m_instance_buffer = vk::create_buffer(m_allocator,
                                          sizeof(SpriteInstanceVertex) * SIM_MAX_AGENTS,
                                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                          VMA_MEMORY_USAGE_CPU_ONLY);

    VK_CHECK(vmaMapMemory(m_allocator, m_instance_buffer.allocation, &m_instance_buffer.data));
    assert(m_instance_buffer.data);

    init_texture_slots();
}

void SpriteRenderer::init_pipeline()
{
    m_pipeline_layout = vk::create_pipeline_layout(m_device, {});

    VkShaderModule vs = vk::load_shader(m_device, "shader/sprite.vert.spv");
    VkShaderModule fs = vk::load_shader(m_device, "shader/sprite.frag.spv");

    m_pipeline = vk::create_gfx_pipeline(m_device,
                                         VK_NULL_HANDLE,
                                         m_render_pass,
                                         m_pipeline_layout,
                                         {{vs, VK_SHADER_STAGE_VERTEX_BIT}, {fs, VK_SHADER_STAGE_FRAGMENT_BIT}});

    vkDestroyShaderModule(m_device, vs, nullptr);
    vkDestroyShaderModule(m_device, fs, nullptr);
}

void SpriteRenderer::deinit()
{
    vkWaitForFences(m_device, m_fences.size(), m_fences.data(), VK_TRUE, ~0ull);

    vmaUnmapMemory(m_allocator, m_instance_buffer.allocation);
    vk::destroy_buffer(m_allocator, m_instance_buffer);

    vkDestroyPipeline(m_device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);

    for (auto pool : m_cmd_pools)
    {
        vkDestroyCommandPool(m_device, pool, nullptr);
    }

    for (auto fence : m_fences)
    {
        vkDestroyFence(m_device, fence, nullptr);
    }

    for (auto buf : m_instance_frame_buffers)
    {
        vk::destroy_buffer(m_allocator, buf);
    }

    vkDestroySemaphore(m_device, m_rel_sem, nullptr);
    vkDestroySemaphore(m_device, m_aq_sem, nullptr);
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
