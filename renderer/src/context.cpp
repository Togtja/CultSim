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

vk::Result RenderContext::init_instance(std::string_view appname, const std::vector<std::string_view>& ext_names)
{
    auto appinfo = vk::ApplicationInfo(appname.data(), 1u, nullptr);

    uint32_t version_avail = 0u;
    auto result = vk::enumerateInstanceVersion(&version_avail);
}

/** --- TESTS --- */

TEST_CASE("RenderContext - Initialization") { auto context = RenderContext{}; }

}  // namespace ulf
