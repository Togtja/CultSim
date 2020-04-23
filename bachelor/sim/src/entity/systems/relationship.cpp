#include "relationship.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Relationship::update(float dt)
{
    CS_AUTOTIMER(Relationship System);
    auto view = m_context.registry->view<component::Relationship>();
    view.each([this](entt::entity e, component::Relationship& relationship) {
        if (relationship.new_create)
        {
            relationship.new_create = false;
        }
    });
}

ISystem* Relationship::clone()
{
    return new Relationship(m_context);
}
} // namespace cs::system
