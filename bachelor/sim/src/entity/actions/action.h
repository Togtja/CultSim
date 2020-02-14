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

    std::vector<Requirement> requirements{};

    float time_to_complete{};
    float time_spent{};

    std::function<void(void)> abort{};
    std::function<void(void)> success{};
    std::function<void(void)> failure{};
};
} // namespace cs::action
