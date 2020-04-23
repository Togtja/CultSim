#pragma once
#include "goal.h"

#include <string>
#include <variant>
#include <vector>

namespace cs::gob
{
class Action
{
public:
    std::string m_name{};
    ETag m_tags{};

    float m_required_time{};
    float m_success_chance{};

    std::variant<sol::function, std::function<float(const Goal)>> m_get_goal_change = [](const Goal goal) {
        /**-1 for undefined */
        return -1;
    };

    std::variant<sol::function, std::function<float()>> m_get_duration = [this]() {

        return m_required_time;
    };

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
} // namespace cs::gob
