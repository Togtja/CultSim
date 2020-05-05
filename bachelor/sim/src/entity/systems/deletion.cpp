#include "deletion.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Deletion::update(float dt)
{
    CS_AUTOTIMER(Deletion System);

    auto view = m_context.registry->view<component::Delete>();
    view.less([this](entt::entity e) {
        m_context.dispatcher->trigger<event::EntityDeleted>({e});
        m_context.registry->destroy(e);
    });
}

ISystem* Deletion::clone()
{
    return new Deletion(m_context);
}
} // namespace cs::system
