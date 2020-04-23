#include "relationship.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include <sol/sol.hpp>

namespace cs::system
{
void Relationship::initialize()
{
    sol::table cultsim = (*m_context.lua_state)["cultsim"];
    cultsim.set_function("get_friendship",
                         [this](entt::entity e, entt::entity other) -> uint8_t { return get_friendship(e, other); });
    cultsim.set_function("get_romace", [this](entt::entity e, entt::entity other) -> uint8_t { return get_romace(e, other); });

    cultsim.set_function("add_friendship", [this](entt::entity e, entt::entity other, uint8_t amount) -> void {
        add_friendship(e, other, amount);
    });
    cultsim.set_function("add_romace",
                         [this](entt::entity e, entt::entity other, uint8_t amount) -> void { add_romace(e, other, amount); });
};

void Relationship::add_agent(entt::entity me)
{
    auto view = m_context.registry->view<component::Relationship>();

    view.each([this, me](entt::entity e, const component::Relationship& relationship) {
        if (me == e)
        {
            // First 8 bits is friendship, last 8 bits is romance
            add_friendship(e, me, relationship.self_friend);
            add_romace(e, me, relationship.self_romace);
            return;
        }
        // TODO: for family add better relationship
        add_friendship(e, me, relationship.default_friend);
        add_romace(e, me, relationship.default_romace);
    });
}

uint8_t Relationship::get_friendship(entt::entity me, entt::entity other)
{
    return (rel_table[me][other] >> 8);
}
void Relationship::add_friendship(entt::entity me, entt::entity other, uint8_t amount)
{
    uint8_t friend_lvl = rel_table[me][other] >> 8;
    if (friend_lvl + amount > 0xFF)
    {
        rel_table[me][other] |= 0xFF00;
    }
    else if (friend_lvl + amount < 0)
    {
        rel_table[me][other] &= 0x00FF;
    }
    else
    {
        rel_table[me][other] += (amount << 8);
    }
}

// Get what "me" feel about the other part
uint8_t Relationship::get_romace(entt::entity me, entt::entity other)
{
    return (rel_table[me][other] & 0x00FF);
}

void Relationship::add_romace(entt::entity me, entt::entity other, uint8_t amount)
{
    uint8_t romace_lvl = rel_table[me][other] & 0x00FF;
    if (romace_lvl + amount > 0xFF)
    {
        rel_table[me][other] |= 0x00FF;
    }
    else if (romace_lvl + amount < 0)
    {
        rel_table[me][other] &= 0xFF00;
    }
    else
    {
        rel_table[me][other] += amount;
    }
}

void Relationship::update(float dt)
{
    CS_AUTOTIMER(Relationship System);
    auto view = m_context.registry->view<component::Relationship>();
    view.each([this, &view](entt::entity e, component::Relationship& relationship) {
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
