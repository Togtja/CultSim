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
        m_context.dispatcher->enqueue<event::RemovedEntity>(event::RemovedEntity{e});
        m_context.registry->destroy(e);
    });
}
void Deletion::check_and_delete(const event::DeleteEntity& event)
{
    m_context.registry->assign<component::Delete>(event.entity);
}
} // namespace cs::system
