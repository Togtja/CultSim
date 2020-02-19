#pragma once

#include "requirement.h"
#include "entity/components/tags.h"

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace cs::action
{
struct Action
{
    std::string name{};

    tags::ETag requirements{};

    float time_to_complete{};
    float time_spent = 0.f;

    std::function<void(void)> abort{};
    std::function<void(void)> success{};
    std::function<void(void)> failure{};
};
} // namespace cs::action
