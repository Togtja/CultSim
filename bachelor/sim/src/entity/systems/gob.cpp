#include "gob.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void GOB::update(float dt)
{
    CS_AUTOTIMER(Goal System);

    auto& registry = *m_context.registry;

    auto view = registry.view<component::Goal, component::Action>();
    view.each([this, dt](entt::entity e, component::Goal& goal, component::Action& action) {
        if (action.current_action.m_name == "")
        {
            /**We are not currently working on any action and must therefore choose a new action*/
        }
    });
}
gob::Action_Sequence GOB::choose_action(std::vector<gob::Action_Sequence> actions, std::vector<gob::Goal> goals)
{
    auto best_action = actions[0];
    auto best_value  = calculate_discontentment(actions[0], goals);
    for (auto action : actions)
    {
        auto value = calculate_discontentment(action, goals);
        if (value < best_value)
        {
            best_value  = value;
            best_action = action;
        }
    }

    return best_action;
}
float GOB::calculate_discontentment(gob::Action_Sequence action, std::vector<gob::Goal> goals)
{
    auto discontentment = 0.f;

    for (auto goal : goals)
    {
        auto value = 0.f;
        if (goal.m_weight_function.index() == 0)
        {
            value += std::get<sol::function>(goal.m_weight_function)().get<float>();
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
            value += std::get<std::function<float(const gob::Goal)>>(action.m_get_goal_change)(goal);
        }

        auto time_value = 0;
        if (action.m_get_duration.index() == 0)
        {
            time_value += std::get<sol::function>(action.m_get_duration)().get<float>();
        }
        else
        {
            time_value += std::get<std::function<float()>>(action.m_get_duration)();
        }

        if (goal.m_change_over_time.index() == 0)
        {
            time_value *= std::get<sol::function>(goal.m_change_over_time)().get<float>();
        }
        else
        {
            time_value *= std::get<std::function<float()>>(goal.m_change_over_time)();
        }

        value += time_value;
    }
    return 0.0f;
}
ISystem* GOB::clone()
{
    return new GOB(m_context);
}
} // namespace cs::system
