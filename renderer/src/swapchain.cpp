/**
 * @file swapchain.cpp
 * @author Carl Findahl
 * @date 07.11.2019
 * @brief The swapchain file contains initialization and creation for the swapchain and all the resources that belong to it.
 */

#include "swapchain.hpp"

namespace ulf
{
Swapchain::Swapchain(vk::Device& device, vk::SurfaceKHR surface) : m_device(device), m_surface(surface)
{
}

Swapchain::~Swapchain() noexcept
{
}

void Swapchain::initialize(vk::PhysicalDevice& pdev, const SwapchainSettings& settings)
{
}

vk::PresentModeKHR Swapchain::select_present_mode(vk::PresentModeKHR desired)
{
}

}  // namespace ulf
