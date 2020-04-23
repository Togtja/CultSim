#include "goal.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Goal::update(float dt)
{
    CS_AUTOTIMER(Deletion System);

    auto& registry = *m_context.registry;

    auto view = registry.view<component::Goal>();
    view.each([this, dt](entt::entity e, component::Goal& goal) {

    });
}

ISystem* Goal::clone()
{
    return new Goal(m_context);
}
} // namespace cs::system
