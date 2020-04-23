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

    });
}
ISystem* GOB::clone()
{
    return new GOB(m_context);
}
} // namespace cs::system
