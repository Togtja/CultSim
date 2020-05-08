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
/**
 * ActionSequence is a Container which holds a series of actions to be executed sequentially
 *
 * ActionSequence holds a list of all actions it will work on, as well as the action it is currently working on. For each action
 * in it, if will run them and handle any errors that the execution of said actions throws. This mostly means that the action
 * system will attempt to backtrack through the list of actions to try again upon one of the actions failing,
 * and if that does not help it will send an error up to the action system.
 * It also holds functions to aid in the choosing of which actions to take, wherein it calculates it's overall impact on all goals
 * via the actions it contains, as well as the time it will take to run all it's actions.
 */
struct ActionSequence
{
    using RunActionsFunction = std::variant<sol::function, std::function<bool(ActionSequence*, entt::entity, std::string*)>>;
    using GoalChangeFunction = std::variant<sol::function, std::function<float(const ActionSequence&, const Goal&)>>;
    using DurationFunction   = std::variant<sol::function, std::function<float(const ActionSequence&, entt::entity e)>>;

public:
    /** Name of the sequence */
    std::string name{};

    /** Tags associated with this action sequence */
    ETag tags{};

    /** The action currently being executed */
    Action current_action;

    /** Actions that are a part of this action sequence */
    std::vector<Action> actions{};

    /**
     * run_actions runs every frame to execute the current action, and upon completion move on to the next action.
     *
     * The function will runs through the action currently stored in current_action, and then moves through the list of stored
     * actions until finishing the final one. It also handles errors thrown by the action, by stepping back up the list of actions
     * to take, in order to try again to complete them. If the first action in the set still fails, it will then throw an error to
     * the action system. The implementation of this action can be done by the user, but the standard implementation should be
     * able to handle going through individual actions easily.
     *
     * @param pself A pointer to the running action sequence itself, needed due to the nature of the lua creation of Action
     * sequences
     * @param e The entity the Action sequence is running on
     * @param error A string that contains any potential errors that may occur during the process of running the function.
     *
     * @return A boolean which tells the action system whether or not we are done running the action sequence
     */
    RunActionsFunction m_run_actions = [](ActionSequence* pself, const entt::entity e, std::string* error) -> bool {
        auto& self = *pself;

        /**As long as we have not completed our action, keep working on it*/
        const auto finished = self.current_action.action(self.current_action, e, *error);

        /**We cannot complete the action*/
        if (!error->empty())
        {
            if (self.current_action == self.actions.back())
            {
                return false;
            }
            else
            {
                for (int i = self.actions.size() - 1; i >= 0; i--)
                {
                    if (self.actions[i] == self.current_action)
                    {
                        self.current_action.flags = 0;
                        self.current_action       = self.actions[i + 1];
                    }
                }
                error->clear();
            }
        }

        /**We have completed the action*/
        else if (finished)
        {
            if (self.current_action == self.actions.front())
            {
                return true;
            }
            const auto itr      = std::find(self.actions.rbegin(), self.actions.rend(), self.current_action);
            self.current_action = *(itr + 1);
        }
        return false;
    };

    /**
     * get_goal_change runs through all stored actions and calculates total change to the goal after running through all actions
     *
     * The function gets the total change to a specific goal after running all functions in the sequence. This is called by the
     * action sequence for all of it's action sequences and all of it's goals, so the functions should be kept short and
     * computationally easy to handle. The implementation presented below is just an example of what could be made, but functions
     * decently well at the given task.
     *
     * @param action_sequence A reference to the action sequence itself
     * @param goal The goal for which all actions will be queried about how they will change it
     *
     * @return The total change in the value of the goal once the action sequence finishes running
     */
    GoalChangeFunction m_get_goal_change = [](const ActionSequence& action_sequence, const Goal& goal) {
        float result = 0;
        for (const auto& action : action_sequence.actions)
        {
            result += action.get_goal_change(goal).get<float>();
        }
        return result;
    };

    /**
     * get_duration runs through all actions and checks how long they will need to complete
     *
     * The function will go through the actions stored within it and tell the action system how long it will take to run through
     * all of it's actions, which is used to determine the desirability of the action sequence. Once more, this is an example
     * implementation, as the function can easily be implemented via lua by the user instead.
     *
     * @param action_sequence A reference to the action sequence itself
     * @param e The entity which the action sequence belongs to
     *
     * @return The total time in seconds it will take the action sequence to be completed
     */
    DurationFunction m_get_duration = [](const ActionSequence& action_sequence, const entt::entity e) {
        float result = 0.0;
        for (const auto& action : action_sequence.actions)
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
