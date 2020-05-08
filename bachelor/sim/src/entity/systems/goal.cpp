#include "goal.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Goal::update(float dt)
{
    CS_AUTOTIMER(Goal System);

    auto view = m_context.registry->view<component::Goal>();
    view.each([dt](component::Goal& goal) {
        for (auto& current_goal : goal.goals)
        {
            current_goal.age += dt;
        }

        /** Puts the most pressing goal in the back */
        std::sort(goal.goals.begin(), goal.goals.end(), [](const gob::Goal& lhs, const gob::Goal& rhs) {
            float lhs_v = 0;
            float rhs_v = 0;

            if (lhs.weight_function.index() == 0)
            {
                lhs_v = std::get<sol::function>(lhs.weight_function)(lhs).get<float>();
            }
            else
            {
                lhs_v = std::get<std::function<float()>>(lhs.weight_function)();
            }

            if (rhs.weight_function.index() == 0)
            {
                rhs_v = std::get<sol::function>(rhs.weight_function)(rhs).get<float>();
            }
            else
            {
                rhs_v = std::get<std::function<float()>>(rhs.weight_function)();
            }

            return lhs_v < rhs_v;
        });
    });
}

ISystem* Goal::clone()
{
    return new Goal(m_context);
}
} // namespace cs::system
