#pragma once
#include "tags.h"

#include <functional>
#include <string>
#include <variant>

#include <entt/entity/registry.hpp>
#include <sol/function.hpp>

namespace cs::gob
{
/** TODO: Fix naming of variables*/

/**
 * The new Actions, which differ slightly from the old implementation in their increased flexibility
 *
 * The new action format allows user to easily write their own requirements directly into the action,
 * as well as affording them a quick and easy way of getting the changes the action will have on goals,
 * which also can be scripted by the user themselves. The goal is to pass more responsibility onto the user in exchange for a
 * significant increase in flexibility.
 */
struct Action
{
    using DurationFunction = std::variant<sol::function, std::function<float(const Action& action, entt::entity e)>>;

    std::string name{};

    /**
     * The Tags of the Action, which can be used by the user to determine which goals the action affects. Could also be used for
     * a number of different things
     */
    ETag tags{};

    /**
     * The time it will take to complete the action. Might be used in conjunction with m_get_duration to get how long the action
     * will take including the time taken for all it's requirements
     */
    float m_required_time{};

    /**
     * A number from 0 - 1 how likely the action is to succeed when finished. This again can be used differently or offer higher
     * precision should that be desireable. How the Variable is used is up to the end user.
     */
    float m_success_chance{};

    /**
     * A set of 32 flag bits which are intended for being used to check if requirements are completed or not, allowing users to
     * avoid more complex if statements for simple bitwise operations. This variable is again flexible, and can be used to store
     * any data a series of boolean could store.
     */
    uint32_t m_flags{};

    /**
     * The actual action the entity takes. This can be potentially anything, though considerations should be made for the
     * implications of large actions on runtimes and performance in general. If used alongside Action sequences via the Action
     * component, this needs to follow the syntax provided by the action sequences run_actions()
     */
    sol::function m_action{};

    /**
     * Function which aids the goal oriented behaviour system in finding the desireability of an action sequence. This can again
     * be written to be anything, but should match the syntax provided by the equivalent function in the action sequence.
     */
    sol::function m_get_goal_change{};

    /**
     * Function which returns the time it will take to complete the action from start to finish. Might be a mathematical formula
     * based on pre-existing conditions, or as simple as returning required_time. Should match syntax required by action sequence.
     */
    DurationFunction m_get_duration = [this](const Action& action, entt::entity e) {
        return action.m_required_time;
    };
};

inline bool operator==(const Action& lhs, const Action& rhs)
{
    if (lhs.name != rhs.name)
    {
        return false;
    }
    if (lhs.m_required_time != rhs.m_required_time)
    {
        return false;
    }
    if (lhs.m_success_chance != rhs.m_success_chance)
    {
        return false;
    }
    if (lhs.tags != rhs.tags)
    {
        return false;
    }
    return true;
}
} // namespace cs::gob
