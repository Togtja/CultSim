/**
 * @file swapchain.cpp
 * @author Carl Findahl
 * @date 07.11.2019
 * @brief The swapchain file contains initialization and creation for the swapchain and all the resources that belong to it.
 */

#include "swapchain.hpp"
#include "context.hpp"
#include "helpers.hpp"

#include <doctest/doctest.h>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL.h>

namespace ulf
{
Swapchain::Swapchain(vk::Device& device, vk::SurfaceKHR surface) : m_device(device), m_surface(surface)
{
}

Swapchain::~Swapchain() noexcept
{
    m_device.destroySwapchainKHR(m_swapchain);
}

void Swapchain::initialize(vk::PhysicalDevice& pdev)
{
    auto settings = SwapchainSettings{};

    /* Enumerate available modes and capabilities */
    auto present_modes = pdev.getSurfacePresentModesKHR(m_surface);
    const auto formats = pdev.getSurfaceFormatsKHR(m_surface);
    const auto capabilities = pdev.getSurfaceCapabilitiesKHR(m_surface);

    /* Select surface format and present mode */
    settings.surface_format = select_surface_format(formats);
    settings.present_mode = select_present_mode(settings.present_mode, present_modes);
    settings.pre_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    settings.image_usage = vk::ImageUsageFlagBits::eColorAttachment;
    settings.image_count = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);
    settings.array_layers = std::min(capabilities.maxImageArrayLayers, 1u);

    initialize(pdev, settings);
}

void Swapchain::initialize(vk::PhysicalDevice& pdev, const SwapchainSettings& settings)
{
    /* Create swapchain from settings */
    auto info = vk::SwapchainCreateInfoKHR({}, m_surface, settings.image_count, settings.surface_format.format,
                                           settings.surface_format.colorSpace, settings.extent, settings.array_layers,
                                           settings.image_usage, vk::SharingMode::eExclusive);

    info.presentMode = settings.present_mode;
    info.preTransform = settings.pre_transform;

    /* Create the swapchain */
    REQUIRE_FALSE(m_swapchain);
    m_swapchain = m_device.createSwapchainKHR(info);
    REQUIRE(m_swapchain);

    /* Extract the images */
    m_images = m_device.getSwapchainImagesKHR(m_swapchain);
    REQUIRE_FALSE(m_images.empty());
}

/* --- TESTS --- */

TEST_CASE("Swapchain Create")
{
    auto context = RenderContext{};
    auto sc = Swapchain(context.device(), context.surface());
    sc.initialize(context.physical_device());
}

}  // namespace ulf
