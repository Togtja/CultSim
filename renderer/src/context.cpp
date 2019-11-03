/**
 * @file context.cpp
 * @author Carl Findahl
 * @date 03.11.2019
 */

#include "context.hpp"

#include <doctest/doctest.h>

namespace ulf
{
RenderContext::RenderContext(const RenderContextSettings& settings) {}

RenderContext::RenderContext(std::string_view appname, const RenderContextSettings& settings) {}

vk::Result RenderContext::init_instance(std::string_view appname, const std::vector<const char*>& layer_names,
                                        const std::vector<const char*>& ext_names)
{
    /* Check available version and require that we have Vulkan 1.1 available */
    uint32_t version_avail = 0u;
    vk::enumerateInstanceVersion(&version_avail);
    REQUIRE(version_avail >= VK_API_VERSION_1_1);

    auto appinfo = vk::ApplicationInfo(appname.data(), 1u, nullptr, 0u, version_avail);

    auto instance_info =
        vk::InstanceCreateInfo({}, &appinfo, layer_names.size(), layer_names.data(), ext_names.size(), ext_names.data());

    m_instance = vk::createInstance(instance_info);
}

/** --- TESTS --- */

TEST_CASE("RenderContext") {
    auto context = RenderContext{};
}

}  // namespace ulf
