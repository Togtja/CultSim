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
/** TODO: More Documentation*/
struct Action
{
    using ADurationFunction = std::variant<sol::function, std::function<float(const Action& action, entt::entity e)>>;

    std::string name{};
    ETag tags{};

    float m_required_time{};
    float m_success_chance{};

    uint32_t m_flags{};

    sol::function m_action{};

    sol::function m_get_goal_change{};

    ADurationFunction m_get_duration = [this](const Action& action, entt::entity e) {
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
