#pragma once
#include "action.h"

#include <memory>
#include <vector>

namespace cs::gob
{
class action_sequence
{
    std::string m_name{};
    ETag m_tags{};

    std::vector<std::unique_ptr<Action>> m_actions{};

    std::variant<sol::function, std::function<float(const Goal)>> m_get_goal_change = [](const Goal goal) {
        /**-1 for undefined */
        float result = 0;
        for (auto& action : m_actions)
        {
            if (action->get_goal_change.index() == 0)
            {
                result += std::get<sol::function>(action->m_get_goal_change)(goal);
            }
            else
            {
                result += std::get<std::function<float(const Goal)>>(action - m_get_goal_change)(goal);
            }
        }
        return result;
    };

    std::variant<sol::function, std::function<float()>> m_get_duration = [this]() {
        float result = 0;
        for (auto& action : actions)
        {
            if (action->get_duration.index() == 0)
            {
                result += std::get<sol::function>(action->get_duration)();
            }
            else
            {
                result += std::get<std::function<float()>>(action->get_duration)();
            }
        }
        return result;
    };

    Action(const Action& other) :
        name(other.name),
        tags(other.tags),
        required_time(other.required_time),
        success_chance(other.success_chance),
        get_goal_change(other.get_goal_change),
    {
        for (const auto& ptr : other.actions)
        {
            actions.emplace_back(ptr->clone());
        }
    }
    Action& operator=(const Action& other)
    {
        /** Protect against self-copy */
        if (this == &other)
        {
            return *this;
        }

        /** Clean up existing members */
        actions.clear();
        tags            = ETag::TAG_None;
        required_time   = 0;
        success_chance  = 0;
        get_goal_change = [](const Goal goal) {
            return 0;
        };
        get_duration = []() {
            return 0;
        };

        /** Copy members from the other */
        tags            = other.tags;
        required_time   = other.required_time;
        success_chance  = other.success_chance;
        get_goal_change = other.get_goal_change;
        get_duration    = other.get_duration;

        for (const auto& ptr : other.actions)
        {
            actions.emplace_back(ptr->clone());
        }

        return *this;
    }

    Action(Action&&) = default;
    Action& operator=(Action&&) = default;
};
} // namespace cs::gob
