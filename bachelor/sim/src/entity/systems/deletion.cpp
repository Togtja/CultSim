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
    view.less([this](entt::entity e) {
        m_context.dispatcher->trigger<event::DeleteEntity>({e});
        m_context.registry->destroy(e);
    });
}

ISystem* Deletion::clone()
{
    return new Deletion(m_context);
}
} // namespace cs::system
