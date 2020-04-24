#pragma once
#include "action.h"
#include "tags.h"

#include <functional>
#include <memory>
#include <vector>

#include <entt/entt.hpp>
#include <sol/sol.hpp>

namespace cs::gob
{
class Action_Sequence
{
public:
    std::string m_name{};
    ETag m_tags{};

    std::vector<std::unique_ptr<Action>> m_actions{};

    Action* current_action;

    std::variant<sol::function, std::function<bool(entt::entity, std::string)>> m_run_actions = [this](const entt::entity e,
                                                                                                       std::string error) {
        bool finished = false;
        if (current_action->m_action.index() == 0)
        {
            finished = std::get<sol::function>(current_action->m_action)(e, error).get<bool>();
        }
        else
        {
            finished = std::get<std::function<bool(entt::entity, std::string&)>>(current_action->m_action)(e, error);
        }

        if (error != "")
        {
            return false;
        }
        else if (finished)
        {
            if (current_action == m_actions.front().get())
            {
                return true;
            }
            else
            {
                for (int i = m_actions.size() - 1; i >= 0; i--)
                {
                    if (current_action == m_actions[i].get())
                    {
                        current_action = m_actions[i - 1].get();
                        break;
                    }
                }
                return false;
            }
        }
    };

    std::variant<sol::function, std::function<float(const Goal)>> m_get_goal_change = [this](const Goal goal) {
        /**-1 for undefined */
        float result = 0;
        for (auto& action : m_actions)
        {
            if (action->m_get_goal_change.index() == 0)
            {
                result += std::get<sol::function>(action->m_get_goal_change)(goal).get<float>();
            }
            else
            {
                result += std::get<std::function<float(const Goal)>>(action->m_get_goal_change)(goal);
            }
        }
        return result;
    };

    std::variant<sol::function, std::function<float()>> m_get_duration = [this]() {
        float result = 0;
        for (auto& action : m_actions)
        {
            if (action->m_get_duration.index() == 0)
            {
                result += std::get<sol::function>(action->m_get_duration)().get<float>();
            }
            else
            {
                result += std::get<std::function<float()>>(action->m_get_duration)();
            }
        }
        return result;
    };

    Action_Sequence() = default;

    Action_Sequence(const Action_Sequence& other) :
        m_name(other.m_name),
        m_tags(other.m_tags),
        m_get_duration(other.m_get_duration),
        m_get_goal_change(other.m_get_goal_change)
    {
        for (const auto& ptr : other.m_actions)
        {
            m_actions.emplace_back(ptr->clone());
        }
    }
    Action_Sequence& operator=(const Action_Sequence& other)
    {
        /** Protect against self-copy */
        if (this == &other)
        {
            return *this;
        }

        /** Clean up existing members */
        m_name = "";
        m_tags = ETag::TAG_None;
        m_actions.clear();
        m_get_goal_change = [](const cs::gob::Goal goal) {
            return 0;
        };
        m_get_duration = []() {
            return 0;
        };

        /** Copy members from the other */
        m_name            = other.m_name;
        m_tags            = other.m_tags;
        m_get_goal_change = other.m_get_goal_change;
        m_get_duration    = other.m_get_duration;

        for (const auto& ptr : other.m_actions)
        {
            m_actions.emplace_back(ptr->clone());
        }

        return *this;
    }

    Action_Sequence(Action_Sequence&&) = default;
    Action_Sequence& operator=(Action_Sequence&&) = default;
};

} // namespace cs::gob
