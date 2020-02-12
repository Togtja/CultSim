#pragma once
#include <spdlog/spdlog.h>

namespace cs::ai
{
struct Action
{
    // TODO: Actually implement actions

    inline void run()
    {
        spdlog::warn("Running action");
    };
};
} // namespace cs::ai
