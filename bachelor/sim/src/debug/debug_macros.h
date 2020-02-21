#pragma once

#include <spdlog/spdlog.h>

namespace cs
{
#ifndef NDEBUG
#    define CS_AMIHERE spdlog::debug("I AM HERE [Ln {} - {}]", __LINE__, __FILE__)
#else
#    define CS_AMIHERE
#endif
} // namespace cs
