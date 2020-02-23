#include "renderer.h"
#include "constants.h"
#include "glutil.h"
#include "vkutil.h"

#include <vector>

#include <SDL_vulkan.h>
#include <spdlog/spdlog.h>

namespace cs::gfx
{
Renderer::~Renderer() noexcept
{
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
    create_surface(window);
    create_renderpass_and_swapchain();

    m_allocator = vk::create_allocator(m_instance, m_device, m_pdevice);

    m_sprite_renderer.init({m_swapchain, m_render_pass, m_gfx_queue_idx, m_gfx_queue, m_allocator});
}

void Renderer::deinit() noexcept
{
    VK_CHECK(vkQueueWaitIdle(m_gfx_queue));
    VK_CHECK(vkDeviceWaitIdle(m_device));
    m_sprite_renderer.deinit();
    vmaDestroyAllocator(m_allocator);

    vk::destroy_swapchain(m_device, m_swapchain);
    vkDestroyRenderPass(m_device, m_render_pass, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyDevice(m_device, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

Renderer::Renderer() : m_sprite_renderer(m_camera)
{
    m_camera.init({0.f, 0.f, 20.f});
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

    const char* layers[] = {
#ifndef NDEBUG
        "VK_LAYER_LUNARG_standard_validation"
#endif
    };
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

void Renderer::create_device()
{
    uint32_t pd_count{};
    vkEnumeratePhysicalDevices(m_instance, &pd_count, nullptr);

    std::vector<VkPhysicalDevice> pdevices(pd_count);
    vkEnumeratePhysicalDevices(m_instance, &pd_count, pdevices.data());

    m_pdevice = pick_physical_device(pdevices);
    assert(m_pdevice);

    m_gfx_queue_idx    = vk::get_queue_index(m_pdevice, VK_QUEUE_GRAPHICS_BIT);
    float priorities[] = {1.f};

    VkDeviceQueueCreateInfo queue_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queue_info.queueFamilyIndex        = m_gfx_queue_idx;
    queue_info.queueCount              = 1;
    queue_info.pQueuePriorities        = priorities;

    std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkPhysicalDeviceFeatures features{};
    features.wideLines         = VK_TRUE;
    features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo create_info      = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    create_info.pQueueCreateInfos       = &queue_info;
    create_info.queueCreateInfoCount    = 1;
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.enabledExtensionCount   = extensions.size();
    create_info.pEnabledFeatures        = &features;

    VK_CHECK(vkCreateDevice(m_pdevice, &create_info, nullptr, &m_device));
    assert(m_device);

    volkLoadDevice(m_device);

    vkGetDeviceQueue(m_device, m_gfx_queue_idx, 0, &m_gfx_queue);
    assert(m_gfx_queue);
}

void Renderer::create_surface(const Window& window)
{
    /** First make a surface */
    SDL_Vulkan_CreateSurface(window.get(), m_instance, &m_surface);
    assert(m_surface);

    VkBool32 support_presentation = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_pdevice, m_gfx_queue_idx, m_surface, &support_presentation);
    assert(support_presentation);
}

void Renderer::create_renderpass_and_swapchain()
{
    /** Enumerate surface formats */
    uint32_t format_count{};
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_pdevice, m_surface, &format_count, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_pdevice, m_surface, &format_count, formats.data());

    VkSurfaceCapabilitiesKHR surface_caps{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pdevice, m_surface, &surface_caps);

    auto format   = vk::select_surface_format(formats);
    m_render_pass = vk::create_render_pass(m_device, format.format);
    m_swapchain   = vk::create_swapchain(m_device, m_pdevice, m_surface, surface_caps, m_render_pass, m_gfx_queue_idx, format);
}

Renderer& get_renderer()
{
    static Renderer r{};
    return r;
}

} // namespace cs::gfx
