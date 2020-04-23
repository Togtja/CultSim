#include "relationship.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Relationship::add_agent(entt::entity me)
{
    auto view = m_context.registry->view<component::Relationship>();

    view.each([this, me](entt::entity e, const component::Relationship& relationship) {
        if (me == e)
        {
        }
    });
}

uint8_t Relationship::get_friendship(entt::entity me, entt::entity other)
{
    return (rel_table[me][other] >> 8);
}

// Get what "me" feel about the other part
uint8_t Relationship::get_romace(entt::entity me, entt::entity other)
{
    return (rel_table[me][other] & 0x00FF);
}
void Relationship::update(float dt)
{
    CS_AUTOTIMER(Relationship System);
    auto view = m_context.registry->view<component::Relationship>();
    view.each([this](entt::entity e, component::Relationship& relationship) {
        if (relationship.new_create)
        {
            add_agent(e);
            relationship.new_create = false;
        }
    });
}

ISystem* Relationship::clone()
{
    return new Relationship(m_context);
}
} // namespace cs::system
