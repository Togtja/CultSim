#pragma once

#include "entity/components/tags.h"

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include <entt/entt.hpp>
#include <sol/sol.hpp>

namespace cs::action
{
struct Action
{
    std::string name{};

    ETag requirements{};

    float time_to_complete{};
    float success_chance = 1.f;

    sol::function success{};
    sol::function failure{};

    float time_spent = 0.f;

    entt::entity target{};

    std::function<void(void)> abort{};

    Action() = default;
};

inline bool operator==(Action const& lhs, Action const& rhs)
{
    return lhs.name == rhs.name;
}
} // namespace cs::action
