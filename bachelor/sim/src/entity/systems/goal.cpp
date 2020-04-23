#include "goal.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Goal::update(float dt)
{
    CS_AUTOTIMER(Goal System);

    auto& registry = *m_context.registry;

    auto view = registry.view<component::Goal>();
    view.each([this, dt](entt::entity e, component::Goal& goal) {
        for (auto current_goal : goal.goals)
        {
            current_goal.m_age += dt;
        }
        // Puts the most pressing goal in the back
        std::sort(goal.goals.begin(), goal.goals.end(), [](const gob::Goal lhs, const gob::Goal rhs) {
            return lhs.m_weight_function < rhs.m_weight_function;
        });
    });
}

ISystem* Goal::clone()
{
    return new Goal(m_context);
}
} // namespace cs::system
