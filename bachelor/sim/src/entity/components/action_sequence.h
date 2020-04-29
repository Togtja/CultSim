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

    Action current_action;

    std::vector<Action> m_actions{};

    std::variant<sol::function, std::function<bool(entt::entity, std::string&)>> m_run_actions = [this](const entt::entity e,
                                                                                                        std::string& error) {

        /**As long as we have not completed our action, keep working on it*/
        auto finished = current_action.m_action(current_action,e, error);

        /**We cannot complete the action*/
        if (!error.empty())
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
                        current_action.m_flags = 0;
                        current_action         = m_actions[i + 1];
                    }
                }
                error.clear();
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
        return false;
    };

    std::variant<sol::function, std::function<float(const Action_Sequence&, const Goal&)>> m_get_goal_change = [this](const Action_Sequence& action_sequence,const Goal& goal) {
        /**-1 for undefined */
        float result = 0;
        for (auto& action : action_sequence.m_actions)
        {
            result += action.m_get_goal_change(goal).get<float>();
        }
        return result;
    };

    std::variant<sol::function, std::function<float(const Action_Sequence&)>> m_get_duration = [this](const Action_Sequence& action_sequence) {
        float result = 0;
        for (auto& action : action_sequence.m_actions)
        {
            if (action.m_get_duration.index() == 0)
            {
                result += std::get<sol::function>(action.m_get_duration)(action).get<float>();
            }
            else
            {
                result += std::get<std::function<float(const Action&)>>(action.m_get_duration)(action);
            }
        }
        return result;
    };
};

} // namespace cs::gob
