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

    uint32_t flags{};

    sol::function m_action{};

    sol::function m_get_goal_change{};

    std::variant<sol::function, std::function<float()>> m_get_duration = [this]() {
        return m_required_time;
    };

    Action() = default;

    Action(std::string name,
           ETag tags,
           float required_time,
           float success_chance,
           sol::function action,
           sol::function get_goal_change,
           std::variant<sol::function, std::function<float()>> get_duration) :
        m_name(name),
        m_tags(tags),
        m_required_time(required_time),
        m_success_chance(success_chance),
        m_action(action),
        m_get_goal_change(get_goal_change),
        m_get_duration(get_duration)
    {
    }
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
