#pragma once

#include "entity/components/tags.h"

#include <string>

#include <entt/entity/entity.hpp>
#include <sol/function.hpp>

/** TODO: Potentially merge with components/actions.h */
/** TODO: Add documentation */

namespace cs::action
{
struct Action
{
    std::string name{};

    ETag requirements{};

    float time_to_complete{};
    float success_chance{1.f};

    sol::function success{};
    sol::function failure{};
    sol::function abort{};

    float time_spent{0.f};

    entt::entity target = entt::null;
};

inline bool operator==(Action const& lhs, Action const& rhs)
{
    return lhs.name == rhs.name && lhs.requirements == rhs.requirements && lhs.time_to_complete == rhs.time_to_complete &&
           lhs.target == rhs.target && lhs.time_spent == rhs.time_spent;
}
} // namespace cs::action
