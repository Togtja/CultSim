#include "gob.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void GOB::update(float dt)
{
    CS_AUTOTIMER(Goal Oriented Behaviour System);

    auto view = m_context.registry->view<component::Goal, component::Action>();
    view.each([this](entt::entity e, const component::Goal& goal, component::Action& action) {
        if (action.current_action_sequence.m_name.empty())
        {
            auto best_action = action.actions[0];
            auto best_value  = calculate_discontentment(e, action.actions[0], goal.goals);
            for (auto action : action.actions)
            {
                auto value = calculate_discontentment(e, action, goal.goals);
                if (value < best_value)
                {
                    best_value  = value;
                    best_action = action;
                }
            }

            action.current_action_sequence                = best_action;
            action.current_action_sequence.current_action = action.current_action_sequence.m_actions.back();
        }
    });
}

ISystem* GOB::clone()
{
    return new GOB(m_context);
}

float GOB::calculate_discontentment(entt::entity e, const gob::ActionSequence& action, const std::vector<gob::Goal>& goals)
{
    auto discontentment = 0.f;

    for (auto goal : goals)
    {
        auto value = 0.f;
        if (goal.m_weight_function.index() == 0)
        {
            value += std::get<sol::function>(goal.m_weight_function)(goal).get<float>();
        }
        else
        {
            value += std::get<std::function<float()>>(goal.m_weight_function)();
        }

        if (action.m_get_goal_change.index() == 0)
        {
            value += std::get<sol::function>(action.m_get_goal_change)(goal).get<float>();
        }
        else
        {
            value += std::get<std::function<float(const gob::ActionSequence&, const gob::Goal&)>>(
                action.m_get_goal_change)(action, goal);
        }

        auto time_value = 0;
        if (action.m_get_duration.index() == 0)
        {
            time_value += std::get<sol::function>(action.m_get_duration)(action, e).get<float>();
        }
        else
        {
            time_value +=
                std::get<std::function<float(const gob::ActionSequence&, entt::entity)>>(action.m_get_duration)(action, e);
        }

        if (goal.m_change_over_time.index() == 0)
        {
            value += std::get<sol::function>(goal.m_change_over_time)(goal, time_value).get<float>();
        }
        else
        {
            value += std::get<std::function<float(const float)>>(goal.m_change_over_time)(time_value);
        }

        if (goal.m_get_discontentment.index() == 0)
        {
            discontentment += std::get<sol::function>(goal.m_get_discontentment)(value).get<float>();
        }
        else
        {
            discontentment += std::get<std::function<float(const float)>>(goal.m_get_discontentment)(value);
        }
    }
    return discontentment;
}
} // namespace cs::system
