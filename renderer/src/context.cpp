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
RenderContext::~RenderContext() noexcept { m_instance.destroy(); }

RenderContext::RenderContext(const RenderContextSettings& settings) : RenderContext("Vulkan Application", settings) {}

RenderContext::RenderContext(std::string_view appname, const RenderContextSettings& settings)
{
    init_instance(appname, settings.instance_layers, settings.instance_ext);
    const auto& physical_devices = m_instance.enumeratePhysicalDevices();
    const auto chosen_device = choose_physical_device(physical_devices);
}

void RenderContext::init_instance(std::string_view appname, const std::vector<const char*>& layer_names,
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

void RenderContext::init_device(const vk::PhysicalDeviceFeatures& features, const std::vector<const char*>& layer_names,
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
}

/** --- TESTS --- */

TEST_CASE("RenderContext")
{
    RenderContextSettings settings{};

    SUBCASE("Default Construction") { auto context = RenderContext{}; }

    SUBCASE("With Validation Layers Only")
    {
        settings.instance_layers = {"VK_LAYER_LUNARG_standard_validation"};
        auto context = RenderContext{settings};
    }

    // SUBCASE("With Validation Layers and Presentation Extensions") { auto context = RenderContext{}; }
}

}  // namespace ulf
