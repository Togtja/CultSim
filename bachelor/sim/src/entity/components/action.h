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

    std::variant<sol::function, std::function<float(const Goal)>> m_get_goal_change = [](const Goal goal) {
        /**-1 for undefined */
        return -1;
    };

    std::variant<sol::function, std::function<float()>> m_get_duration = [this]() {
        return m_required_time;
    };

    std::variant<sol::function, std::function<bool(entt::entity, std::string&)>> m_action = [this](entt::entity e,
                                                                                                   std::string& error) {
        error = fmt::format("Undefined action {} for entity {}, please define an m_action function.", m_name, e);
        return false;
    };

    Action() = default;

    Action(std::string name,
           ETag tags,
           float required_time,
           float success_chance,
           std::variant<sol::function, std::function<float(const Goal)>> get_goal_change,
           std::variant<sol::function, std::function<float()>> get_duration) :
        m_name(name),
        m_tags(tags),
        m_required_time(required_time),
        m_success_chance(success_chance),
        m_get_goal_change(get_goal_change),
        m_get_duration(get_duration)
    {
    }

    Action* clone()
    {
        return new Action(m_name, m_tags, m_required_time, m_success_chance, m_get_goal_change, m_get_duration);
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
    if (lhs.m_action != rhs.m_action)
    {
        return false;
    }
    if (lhs.m_get_duration != rhs.m_get_duration)
    {
        return false;
    }
    if (lhs.m_get_goal_change != rhs.m_get_goal_change)
    {
        return false;
    }
    return true;
}
} // namespace cs::gob
