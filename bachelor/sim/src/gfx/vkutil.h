#pragma once
#include "glutil.h"

#include <volk.h>

namespace cs::vk
{
VkRenderPass create_render_pass(VkDevice device);

VkImageView create_image_view(VkDevice device, VkImage image);

VkFramebuffer create_framebuffer(VkDevice device, VkRenderPass rp, VkImageView image_view, VkExtent2D extent);
} // namespace cs::vk
