#pragma once

#include "entity/components/tags.h"

#include <string>

#include <entt/entity/entity.hpp>
#include <sol/function.hpp>

namespace cs::action
{
/**
 * An action holds all parameters needed for the action system to be able to correctly run said action.
 */
struct Action
{
    std::string name{};

    /**
     * The requirements that need to be fullfilled before the action can be completed.
     * These tags will be used by the action system to assign requirement components to the entity in question.
     * Said requirements will be based on the strategy containing this action, meaning that some actions may have different
     * effects in different strategies
     */
    ETag requirements{};

    float time_to_complete{};
    /**
     * The odds (from 0 - 1) of the action succeeding upon all it's parameters being completed
     */
    float success_chance{1.f};

    /**
     * Actions which are run depending on whether the actions finishes successfully or has to be interrupted before completion
     */
    sol::function success{};
    sol::function failure{};
    sol::function abort{};

    float time_spent{0.f};

    /**
     * The target of the actions. May be the entity running the action in some cases. Should always be a valid entity.
     */
    entt::entity target = entt::null;
};

inline bool operator==(Action const& lhs, Action const& rhs)
{
    return lhs.name == rhs.name && lhs.requirements == rhs.requirements && lhs.time_to_complete == rhs.time_to_complete &&
           lhs.target == rhs.target && lhs.time_spent == rhs.time_spent;
}
} // namespace cs::action
