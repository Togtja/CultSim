#pragma once

#include "requirement.h"

#include <functional>
#include <string>
#include <vector>

namespace cs::action
{
struct Action
{
    std::string name{};

    std::vector<IRequirement> requirements{};

    float time_to_complete{};
    float time_spent{};

    std::function<void> abort{};
    std::function<void> success{};
    std::function<void> failure{};
};
} // namespace cs::action
