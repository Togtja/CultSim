#pragma once
#include "action.h"
#include "goal.h"
#include "tags.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <entt/entt.hpp>
#include <sol/sol.hpp>

namespace cs::gob
{
/** TODO: Change variable names*/
/** TODO: More documentation*/
struct ActionSequence
{
    using RunActionsFunction = std::variant<sol::function, std::function<bool(ActionSequence&, entt::entity, std::string*)>>;
    using GoalChangeFunction = std::variant<sol::function, std::function<float(const ActionSequence&, const Goal&)>>;
    using DurationFunction   = std::variant<sol::function, std::function<float(const ActionSequence&, entt::entity e)>>;

public:
    std::string m_name{};
    ETag m_tags{};

    Action current_action;

    std::vector<Action> m_actions{};

    RunActionsFunction m_run_actions = [this](ActionSequence& self, const entt::entity e, std::string* error) -> bool {
        /**As long as we have not completed our action, keep working on it*/
        const auto finished = self.current_action.m_action(self.current_action, e, error);

        /**We cannot complete the action*/
        if (!error->empty())
        {
            if (self.current_action == self.m_actions.back())
            {
                return false;
            }
            else
            {
                for (int i = self.m_actions.size() - 1; i >= 0; i--)
                {
                    if (self.m_actions[i] == self.current_action)
                    {
                        self.current_action.m_flags = 0;
                        self.current_action         = self.m_actions[i + 1];
                    }
                }
                error->clear();
            }
        }

        /**We have completed the action*/
        else if (finished)
        {
            if (self.current_action == self.m_actions.front())
            {
                return true;
            }
            const auto itr      = std::find(self.m_actions.rbegin(), self.m_actions.rend(), self.current_action);
            self.current_action = *(itr + 1);
        }
        return false;
    };

    GoalChangeFunction m_get_goal_change = [this](const ActionSequence& action_sequence, const Goal& goal) {
        float result = 0;
        for (const auto& action : action_sequence.m_actions)
        {
            result += action.m_get_goal_change(goal).get<float>();
        }
        return result;
    };

    DurationFunction m_get_duration = [this](const ActionSequence& action_sequence, const entt::entity e) {
        float result = 0.0;
        for (const auto& action : action_sequence.m_actions)
        {
            if (action.m_get_duration.index() == 0)
            {
                result += std::get<sol::function>(action.m_get_duration)(action, e).get<float>();
            }
            else
            {
                result += std::get<std::function<float(const Action&, entt::entity)>>(action.m_get_duration)(action, e);
            }
        }
        return result;
    };
};

} // namespace cs::gob
