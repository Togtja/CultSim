#include "vkutil.h"

namespace cs::vk
{
uint32_t get_queue_index(VkPhysicalDevice pdev, VkQueueFlags required_flags)
{
    /** Get the queues of the device and identify a graphics and present queue */
    uint32_t count{};
    vkGetPhysicalDeviceQueueFamilyProperties(pdev, &count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_info(count);
    vkGetPhysicalDeviceQueueFamilyProperties(pdev, &count, queue_info.data());

    /** Identify all queue families */
    uint32_t idx = 0u;

    /** Look for dedicated queue that support flag(s) */
    for (const auto& queue : queue_info)
    {
        if ((queue.queueFlags & required_flags) == required_flags && !(queue.queueFlags & ~required_flags))
        {
            return idx;
        }
        ++idx;
    }

    /** Look for generic queue that support flag(s) */
    idx = 0u;
    for (const auto& queue : queue_info)
    {
        if ((queue.queueFlags & required_flags))
        {
            return idx;
        }
        ++idx;
    }

    return idx;
}

VkPresentModeKHR select_present_mode(VkPresentModeKHR desired, std::vector<VkPresentModeKHR>& avail)
{
    /* Map of priorities for each present mode when we must fall back */
    auto priorities = std::unordered_map<VkPresentModeKHR, int>{{VK_PRESENT_MODE_FIFO_KHR, 10},
                                                                {VK_PRESENT_MODE_FIFO_RELAXED_KHR, 8},
                                                                {VK_PRESENT_MODE_MAILBOX_KHR, 9},
                                                                {VK_PRESENT_MODE_IMMEDIATE_KHR, 5},
                                                                {VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR, 3},
                                                                {VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR, 2}};

    /* Ensure the desired mode has highest priority */
    priorities[desired] = 11;

    /* Sort enough so we know what mode is the best one, then that is the one we should use */
    std::nth_element(avail.begin(), avail.begin(), avail.end(), [&priorities](auto a, auto b) {
        return priorities[a] > priorities[b];
    });

    return avail.front();
}

VkSurfaceFormatKHR select_surface_format(const std::vector<VkSurfaceFormatKHR>& avail)
{
    for (auto format : avail)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }

    return avail[0];
}

VkRenderPass create_render_pass(VkDevice device, VkFormat format)
{
    VkAttachmentReference attachment_reference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpass_info = {};
    subpass_info.pColorAttachments    = &attachment_reference;
    subpass_info.colorAttachmentCount = 1;
    subpass_info.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;

    VkAttachmentDescription attachments[1] = {};
    attachments[0].flags                   = 0;
    attachments[0].format                  = format;
    attachments[0].samples                 = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].initialLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].finalLayout             = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[0].loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    VkRenderPassCreateInfo render_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    render_info.attachmentCount        = 1;
    render_info.pAttachments           = attachments;
    render_info.pSubpasses             = &subpass_info;
    render_info.subpassCount           = 1;

    VkRenderPass out{VK_NULL_HANDLE};
    VK_CHECK(vkCreateRenderPass(device, &render_info, nullptr, &out));
    assert(out);

    return out;
}

VkImageView create_image_view(VkDevice device, VkImage image, VkFormat format)
{
    VkImageViewCreateInfo create_info       = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    create_info.image                       = image;
    create_info.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format                      = format;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.layerCount = 1;
    create_info.subresourceRange.levelCount = 1;

    VkImageView out{VK_NULL_HANDLE};
    VK_CHECK(vkCreateImageView(device, &create_info, nullptr, &out));
    assert(out);

    return out;
}

VkFramebuffer create_framebuffer(VkDevice device, VkRenderPass rp, VkImageView image_view, VkExtent2D extent)
{
    VkFramebufferCreateInfo create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    create_info.attachmentCount         = 1;
    create_info.pAttachments            = &image_view;
    create_info.width                   = extent.width;
    create_info.height                  = extent.height;
    create_info.renderPass              = rp;
    create_info.layers                  = 1;

    VkFramebuffer out{VK_NULL_HANDLE};
    VK_CHECK(vkCreateFramebuffer(device, &create_info, nullptr, &out));
    assert(out);

    return out;
}

VkSemaphore create_semaphore(VkDevice device)
{
    VkSemaphoreCreateInfo create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkSemaphore out{VK_NULL_HANDLE};
    VK_CHECK(vkCreateSemaphore(device, &create_info, nullptr, &out));
    assert(out);

    return out;
}

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_index, VkCommandPoolCreateFlags flags)
{
    VkCommandPoolCreateInfo create_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    create_info.queueFamilyIndex        = queue_index;
    create_info.flags |= flags;

    VkCommandPool out{VK_NULL_HANDLE};
    VK_CHECK(vkCreateCommandPool(device, &create_info, nullptr, &out));
    assert(out);

    return out;
}

VkCommandBuffer begin_one_time_cmd_buffer(VkDevice device, VkCommandPool pool)
{
    VkCommandBufferAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocate_info.commandBufferCount          = 1;
    allocate_info.commandPool                 = pool;
    allocate_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer out{VK_NULL_HANDLE};
    VK_CHECK(vkAllocateCommandBuffers(device, &allocate_info, &out));
    assert(out);

    VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(out, &begin_info));
    return out;
}

void end_one_time_cmd_buffer(VkCommandBuffer buffer)
{
    vkEndCommandBuffer(buffer);
}

} // namespace cs::vk
