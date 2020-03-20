#include "deletion.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Deletion::update(float dt)
{
    CS_AUTOTIMER(Deletion System);

    auto& registry = *m_context.registry;

    auto view = registry.view<component::Delete>();
    view.each([this, dt](entt::entity e, component::Delete& deletion) {
        deletion.time_since_marked += dt;

        if (deletion.time_since_marked >= deletion.time_before_timeout)
        {
            m_context.registry->remove<component::Delete>(e);
        }
    });
}
void Deletion::check_and_delete(const event::DeleteEntity& event)
{
    if (m_context.registry->has<component::Delete>(event.entity)) 
    {
        m_context.dispatcher->enqueue<event::RemovedEntity>(event::RemovedEntity{event.entity});
        m_context.registry->destroy(event.entity);
    }
}
} // namespace cs::system