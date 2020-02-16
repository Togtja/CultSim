#include "renderer.h"
#include "constants.h"
#include "glutil.h"

#include <vector>

#include <SDL_vulkan.h>
#include <spdlog/spdlog.h>

namespace cs::gfx
{
Renderer::~Renderer() noexcept
{
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyDevice(m_device, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

DebugRenderer& Renderer::debug()
{
    return m_debug_renderer;
}

const DebugRenderer& Renderer::debug() const
{
    return m_debug_renderer;
}

SpriteRenderer& Renderer::sprite()
{
    return m_sprite_renderer;
}

const SpriteRenderer& Renderer::sprite() const
{
    return m_sprite_renderer;
}

void Renderer::set_camera_position(glm::vec3 pos)
{
    /** Todo */
}

void Renderer::move_camera(glm::vec3 delta)
{
    m_camera.move(delta);
}

void Renderer::set_camera_bounds(glm::vec2 bounds)
{
    m_camera.set_boundaries(bounds);
}

void Renderer::init(const Window& window)
{
    create_instance(window);
    create_device();
    create_swapchain(window);
}

Renderer::Renderer() : m_sprite_renderer(m_camera)
{
    m_camera.init({0.f, 0.f, 0.f});
}

void Renderer::create_instance(const Window& window)
{
    VK_CHECK(volkInitialize());

    VkApplicationInfo appinfo  = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appinfo.apiVersion         = VK_API_VERSION_1_1;
    appinfo.pApplicationName   = PROGRAM_NAME.data();
    appinfo.applicationVersion = VK_MAKE_VERSION(PROGRAM_VERSION_MAJOR, PROGRAM_VERSION_MINOR, PROGRAM_VERSION_PATCH);

    VkInstanceCreateInfo instance_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instance_info.pApplicationInfo     = &appinfo;

    const char* layers[]              = {"VK_LAYER_LUNARG_standard_validation"};
    instance_info.ppEnabledLayerNames = layers;
    instance_info.enabledLayerCount   = sizeof(layers) / sizeof(layers[0]);

    uint32_t ext_count{};
    SDL_Vulkan_GetInstanceExtensions(window.get(), &ext_count, nullptr);

    std::vector<const char*> extensions(ext_count);
    SDL_Vulkan_GetInstanceExtensions(window.get(), &ext_count, extensions.data());

    instance_info.ppEnabledExtensionNames = extensions.data();
    instance_info.enabledExtensionCount   = ext_count;

    VK_CHECK(vkCreateInstance(&instance_info, nullptr, &m_instance));
    volkLoadInstance(m_instance);

    spdlog::info("created Vulkan instance");
}

VkPhysicalDevice Renderer::pick_physical_device(const std::vector<VkPhysicalDevice>& available)
{
    for (auto pdev : available)
    {
        VkPhysicalDeviceProperties2 properties = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        vkGetPhysicalDeviceProperties2(pdev, &properties);

        if (properties.properties.apiVersion >= VK_API_VERSION_1_1 &&
            properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            spdlog::info("using gpu {}", properties.properties.deviceName);
            return pdev;
        }
    }

    if (!available.empty())
    {
        VkPhysicalDeviceProperties2 properties = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        vkGetPhysicalDeviceProperties2(available.front(), &properties);
        spdlog::warn("using backup gpu {}", properties.properties.deviceName);
        return available.front();
    }

    spdlog::critical("no Vulkan supported gpu's!");
    return VK_NULL_HANDLE;
}

uint32_t Renderer::get_queue_index(VkPhysicalDevice pdev, VkQueueFlags required_flags)
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

void Renderer::create_device()
{
    uint32_t pd_count{};
    vkEnumeratePhysicalDevices(m_instance, &pd_count, nullptr);

    std::vector<VkPhysicalDevice> pdevices(pd_count);
    vkEnumeratePhysicalDevices(m_instance, &pd_count, pdevices.data());

    m_pdevice = pick_physical_device(pdevices);
    assert(m_pdevice);

    m_gfx_queue_idx    = get_queue_index(m_pdevice, VK_QUEUE_GRAPHICS_BIT);
    float priorities[] = {1.f};

    VkDeviceQueueCreateInfo queue_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queue_info.queueFamilyIndex        = m_gfx_queue_idx;
    queue_info.queueCount              = 1;
    queue_info.pQueuePriorities        = priorities;

    const char* extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkPhysicalDeviceFeatures features{};
    features.wideLines         = VK_TRUE;
    features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo create_info      = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    create_info.pQueueCreateInfos       = &queue_info;
    create_info.queueCreateInfoCount    = 1;
    create_info.ppEnabledExtensionNames = extensions;
    create_info.enabledExtensionCount   = 1;
    create_info.pEnabledFeatures        = &features;

    VK_CHECK(vkCreateDevice(m_pdevice, &create_info, nullptr, &m_device));
    assert(m_device);

    volkLoadDevice(m_device);
}

void Renderer::create_swapchain(const Window& window)
{
    /** First make a surface */
    SDL_Vulkan_CreateSurface(window.get(), m_instance, &m_surface);
    assert(m_surface);

    VkBool32 support_presentation = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_pdevice, m_gfx_queue_idx, m_surface, &support_presentation);
    assert(support_presentation);

    /** Then swapchain */
    VkSwapchainCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    create_info.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.surface                  = m_surface;
    create_info.preTransform             = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    create_info.presentMode              = VK_PRESENT_MODE_FIFO_KHR;
    create_info.imageExtent              = {1280, 720};
    create_info.imageFormat              = VK_FORMAT_B8G8R8A8_UNORM;
    create_info.minImageCount            = 2;
    create_info.queueFamilyIndexCount    = 1;
    create_info.pQueueFamilyIndices      = &m_gfx_queue_idx;
    create_info.imageColorSpace          = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    create_info.imageSharingMode         = VK_SHARING_MODE_EXCLUSIVE;
    create_info.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.imageArrayLayers         = 1;

    VK_CHECK(vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swapchain));
    assert(m_swapchain);

    uint32_t image_count{};
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr);

    std::vector<VkImage> images(image_count);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, images.data());
    m_swapchain_images = std::move(images);
    assert(image_count);
}

Renderer& get_renderer()
{
    static Renderer r{};
    return r;
}

} // namespace cs::gfx
