/**
 * @file context.cpp
 * @author Carl Findahl
 * @date 03.11.2019
 */

#include "context.hpp"
#include "helpers.hpp"

#include <cstring>
#include <algorithm>

#include <doctest/doctest.h>

namespace ulf
{
RenderContext::~RenderContext() noexcept
{
    m_device.destroy();
    m_instance.destroy();
}

vk::Result RenderContext::initialize(std::string_view appname, const RenderContextSettings& settings)
{
    /* Initialize the instance */
    if (const auto res = init_instance(appname, settings.instance_layers, settings.instance_ext); res != vk::Result::eSuccess)
    {
        return res;
    }

    /* Choose a physical device */
    const auto& physical_devices = m_instance.enumeratePhysicalDevices();
    m_pdevice = choose_physical_device(physical_devices);

    /* Initialize the logical device */
    const auto res = init_device(settings.device_features, settings.device_layers, settings.device_ext);
    return res;
}

vk::Device RenderContext::device() const
{
    return m_device;
}

vk::Queue RenderContext::gfx_queue() const
{
    return m_gfx_queue;
}

uint32_t RenderContext::gfx_idx() const
{
    return get_queue_index(m_pdevice, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer);
}

vk::Queue RenderContext::compute_queue() const
{
    return m_compute_queue;
}

uint32_t RenderContext::compute_idx() const
{
    return get_queue_index(m_pdevice, vk::QueueFlagBits::eCompute);
}

vk::Result RenderContext::init_instance(std::string_view appname, const std::vector<const char*>& layer_names,
                                        const std::vector<const char*>& ext_names)
{
    CHECK_MESSAGE(appname != "", "The appname should not be empty.");

    /* Check available version and require that we have Vulkan 1.1 available */
    uint32_t version_avail = 0u;
    vk::enumerateInstanceVersion(&version_avail);
    REQUIRE(version_avail >= VK_API_VERSION_1_1);

    /* Application Info Struct */
    const auto appinfo = vk::ApplicationInfo(appname.data(), 1u, nullptr, 0u, version_avail);

    /* Get Available Layers */
    const auto layers_avail = vk::enumerateInstanceLayerProperties();
    std::vector<const char*> layers = filter_desired_to_available_layers(layer_names, layers_avail);
    CHECK(layers.size() >= layer_names.size());

    /* Get available Extensions */
    auto ext_avail = vk::enumerateInstanceExtensionProperties();
    std::vector<const char*> extensions = filter_desired_to_available_extensions(ext_names, ext_avail);
    CHECK(extensions.size() >= ext_names.size());

    /* Fill inn creation info */
    const auto info = vk::InstanceCreateInfo({}, &appinfo, layers.size(), layers.data(), extensions.size(), extensions.data());

    /* We must not have an instance before creation, but must after */
    REQUIRE_FALSE(m_instance);
    m_instance = vk::createInstance(info);
    REQUIRE(m_instance);

    /* TODO : Not always success */
    return vk::Result::eSuccess;
}

vk::PhysicalDevice RenderContext::choose_physical_device(const std::vector<vk::PhysicalDevice>& devices)
{
    REQUIRE_FALSE(devices.empty());

    /* With only one device, it has to be the best one */
    if (devices.size() == 1u)
    {
        return devices.front();
    }

    /* Otherwise rate them */
    auto best = std::max(devices.cbegin(), devices.cend(),
                         [](const auto& a, const auto& b) { return rate_physical_device(*a) > rate_physical_device(*b); });

    /* There has to be at least one that is "the best" */
    REQUIRE(best != devices.end());
    return *best;
}

vk::Result RenderContext::init_device(const vk::PhysicalDeviceFeatures& features, const std::vector<const char*>& layer_names,
                                      const std::vector<const char*>& ext_names)
{
    /* Get Available Layers */
    const auto layers_avail = m_pdevice.enumerateDeviceLayerProperties();
    std::vector<const char*> layers = filter_desired_to_available_layers(layer_names, layers_avail);
    CHECK(layers.size() >= layer_names.size());

    /* Get Available Extensions */
    auto ext_avail = m_pdevice.enumerateDeviceExtensionProperties();
    std::vector<const char*> extensions = filter_desired_to_available_extensions(ext_names, ext_avail);
    CHECK(extensions.size() >= ext_names.size());

    /* Get queue indices for the two queues that we will support */
    const auto compute_queue_index = compute_idx();
    const auto gfx_queue_index = gfx_idx();

    /* All queues have equal priority */
    const auto priorities = 1.f;
    const auto queue_info = std::array{vk::DeviceQueueCreateInfo{{}, compute_queue_index, 1u, &priorities},
                                       vk::DeviceQueueCreateInfo{{}, gfx_queue_index, 1u, &priorities}};

    /* Device Create Info */
    const auto info = vk::DeviceCreateInfo({}, queue_info.size(), queue_info.data(), layers.size(), layers.data(),
                                           extensions.size(), extensions.data(), &features);

    /* Create Device */
    REQUIRE_FALSE(m_device);
    m_device = m_pdevice.createDevice(info);
    REQUIRE(m_device);

    /* Extract queues from device */
    m_gfx_queue = m_device.getQueue(gfx_queue_index, 0u);
    m_compute_queue = m_device.getQueue(compute_queue_index, 0u);
    REQUIRE(m_gfx_queue);
    REQUIRE(m_compute_queue);

    /* TODO : Not always success */
    return vk::Result::eSuccess;
}

/** --- TESTS --- */

TEST_CASE("RenderContext")
{
    RenderContextSettings settings{};

    SUBCASE("Default Construction")
    {
        auto context = RenderContext{};
        context.initialize("Test App", settings);
    };
}

}  // namespace ulf
