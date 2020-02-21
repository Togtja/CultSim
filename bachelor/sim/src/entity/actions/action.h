#pragma once

#include "entity/components/tags.h"

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "entt/entt.hpp"

namespace cs::action
{
struct Action
{
    std::string name{};

    ETag requirements{};

    float time_to_complete{};
    float time_spent = 0.f;

    entt::entity target{};

    std::function<void(entt::entity owner, entt::entity target, entt::registry& registry)> success{};
    std::function<void(entt::entity target, entt::registry& registry)> failure{};
    std::function<void(void)> abort{};
};

inline bool operator==(Action const& lhs, Action const& rhs)
{
    return lhs.name == rhs.name;
}
} // namespace cs::action
