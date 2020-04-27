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

    std::vector<Action> m_actions{};

    Action current_action;

    std::variant<sol::function, std::function<bool(entt::entity, std::string)>> m_run_actions = [this](const entt::entity e,
                                                                                                       std::string& error) {
        bool finished = false;

        /**As long as we have not completed our action, keep working on it*/
        current_action.m_action(e, error);

        /**We cannot complete the action*/
        if (error == "")
        {
            if (current_action == m_actions.back())
            {
                return false;
            }
            else
            {
                for (int i = m_actions.size() - 1; i >= 0; i--)
                {
                    if (m_actions[i] == current_action)
                    {
                        current_action.flags = 0;
                        current_action       = m_actions[i + 1];
                    }
                }
            }
        }
        /**We have completed the action*/
        else if (finished)
        {
            if (current_action == m_actions.front())
            {
                return true;
            }
            else
            {
                for (int i = m_actions.size() - 1; i >= 0; i--)
                {
                    if (current_action == m_actions[i])
                    {
                        current_action = m_actions[i - 1];
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
            result += action.m_get_goal_change(goal).get<float>();
        }
        return result;
    };

    std::variant<sol::function, std::function<float()>> m_get_duration = [this]() {
        float result = 0;
        for (auto& action : m_actions)
        {
            if (action.m_get_duration.index() == 0)
            {
                result += std::get<sol::function>(action.m_get_duration)().get<float>();
            }
            else
            {
                result += std::get<std::function<float()>>(action.m_get_duration)();
            }
        }
        return result;
    };
};

} // namespace cs::gob
