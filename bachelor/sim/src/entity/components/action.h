#pragma once
#include "goal.h"
#include "tags.h"

#include <functional>
#include <string>
#include <variant>

#include <entt/entt.hpp>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace cs::gob
{
class Action
{
public:
    std::string m_name{};
    ETag m_tags{};

    float m_required_time{};
    float m_success_chance{};

    uint32_t m_flags{};

    sol::function m_action{};

    sol::function m_get_goal_change{};

    std::variant<sol::function, std::function<float(const Action& action, entt::entity e)>> m_get_duration = [this](const Action& action,entt::entity e) {
        return action.m_required_time;
    };
};

inline bool operator==(const Action& lhs, const Action& rhs)
{
    if (lhs.m_name != rhs.m_name)
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
    if (lhs.m_tags != rhs.m_tags)
    {
        return false;
    }
    return true;
}
} // namespace cs::gob
