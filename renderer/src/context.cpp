/**
 * @file context.cpp
 * @author Carl Findahl
 * @date 03.11.2019
 */

#include "context.hpp"

#include <doctest/doctest.h>

namespace ulf
{
RenderContext::~RenderContext() noexcept { m_instance.destroy(); }

RenderContext::RenderContext(const RenderContextSettings& settings) : RenderContext("Vulkan Application", settings) {}

RenderContext::RenderContext(std::string_view appname, const RenderContextSettings& settings)
{
    init_instance(appname, settings.instance_layers, settings.instance_ext);
}

void RenderContext::init_instance(std::string_view appname, const std::vector<const char*>& layer_names,
                                  const std::vector<const char*>& ext_names)
{
    /* Test Pre-Conditions */
    CHECK_MESSAGE(appname != "", "The appname should not be empty.");

    /* Check available version and require that we have Vulkan 1.1 available */
    uint32_t version_avail = 0u;
    vk::enumerateInstanceVersion(&version_avail);
    REQUIRE(version_avail >= VK_API_VERSION_1_1);

    /* Provide information and create instance */
    auto appinfo = vk::ApplicationInfo(appname.data(), 1u, nullptr, 0u, version_avail);
    auto info = vk::InstanceCreateInfo({}, &appinfo, layer_names.size(), layer_names.data(), ext_names.size(), ext_names.data());

    /* We must not have an instance before creation, but must after */
    REQUIRE(!m_instance);
    m_instance = vk::createInstance(info);
    REQUIRE(m_instance);
}

/** --- TESTS --- */

TEST_CASE("RenderContext") { auto context = RenderContext{}; }

}  // namespace ulf
