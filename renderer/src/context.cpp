/**
 * @file context.cpp
 * @author Carl Findahl
 * @date 03.11.2019
 */

#include "context.hpp"

#include <doctest/doctest.h>

namespace ulf
{
RenderContext::RenderContext() { init_instance("Vulkan Renderer"); }

RenderContext::RenderContext(std::string_view appname) { init_instance(appname); }

vk::Result RenderContext::init_instance(std::string_view appname) {
    const auto appinfo = vk::ApplicationInfo(appname.data(), 1u, nullptr, 0, VK_API_VERSION_1_1);
//    vk::createInstance()
}

/** --- TESTS --- */

TEST_CASE("RenderContext - Initialization") { auto context = RenderContext{}; }

}  // namespace ulf
