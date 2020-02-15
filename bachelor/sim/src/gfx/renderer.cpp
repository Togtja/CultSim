#include "renderer.h"
#include "constants.h"
#include "glutil.h"

#include <spdlog/spdlog.h>

namespace cs::gfx
{
Renderer::~Renderer() noexcept
{
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

Renderer::Renderer() : m_sprite_renderer(m_camera)
{
    m_camera.init({0.f, 0.f, 0.f});

    create_instance();
    create_device();
    create_swapchain();
}

void Renderer::create_instance()
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

    const char* extensions[]              = {VK_KHR_SURFACE_EXTENSION_NAME};
    instance_info.ppEnabledExtensionNames = extensions;
    instance_info.enabledExtensionCount   = sizeof(extensions) / sizeof(extensions[0]);

    VK_CHECK(vkCreateInstance(&instance_info, nullptr, &m_instance));
    volkLoadInstance(m_instance);

    spdlog::info("created Vulkan instance");
}

VkPhysicalDevice Renderer::pick_physical_device()
{
    return nullptr;
}

void Renderer::create_device()
{
}

void Renderer::create_swapchain()
{
}

Renderer& get_renderer()
{
    static Renderer r{};
    return r;
}
} // namespace cs
