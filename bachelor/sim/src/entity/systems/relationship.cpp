#include "relationship.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include <sol/sol.hpp>

namespace cs::system
{
Relationship::Relationship(const Relationship& other) : m_rel_table(other.m_rel_table), ISystem(other)
{
    m_parents_reg = other.m_parents_reg.clone();
}

Relationship& Relationship::operator=(const Relationship& other)
{
    if (&other == this)
    {
        return *this;
    }
    m_context     = other.m_context;
    m_rel_table   = other.m_rel_table;
    m_parents_reg = other.m_parents_reg.clone();
    return *this;
}

void Relationship::initialize()
{
    m_context.dispatcher->sink<event::BornEntity>().connect<&Relationship::new_child_to_reg>(*this);
    sol::table cultsim = (*m_context.lua_state)["cultsim"];
    cultsim.set_function("get_friendship",
                         [this](entt::entity e, entt::entity other) -> uint8_t { return get_friendship(e, other); });
    cultsim.set_function("get_romance", [this](entt::entity e, entt::entity other) -> uint8_t { return get_romance(e, other); });

    cultsim.set_function("add_friendship", [this](entt::entity e, entt::entity other, uint8_t amount) -> void {
        add_friendship(e, other, amount);
    });
    cultsim.set_function("add_romance",
                         [this](entt::entity e, entt::entity other, uint8_t amount) -> void { add_romance(e, other, amount); });
};

void Relationship::deinitialize()
{
    m_context.dispatcher->sink<event::BornEntity>().disconnect<&Relationship::new_child_to_reg>(*this);
}

void Relationship::new_child_to_reg(const event::BornEntity& event)
{
    auto new_entt = m_parents_reg.create();
    m_context.registry->visit([this, event, new_entt](uint32_t comp_id) {
        if (comp_id == entt::type_info<component::Name>::id())
        {
            m_parents_reg.assign<component::Name>(new_entt, m_context.registry->get<component::Name>(event.new_born));
        }
        if (comp_id == entt::type_info<component::Relationship>::id())
        {
            auto rel_c = m_context.registry->get<component::Relationship>(event.new_born);
            if (rel_c.parent_1.first != entt::null && rel_c.parent_2.first != entt::null)
            {
                component::Relationship fam;
                fam.old_id         = event.new_born;
                fam.parent_1.first = rel_c.parent_1.first;
                fam.parent_2.first = rel_c.parent_2.first;

                auto view = m_parents_reg.view<component::Relationship>();
                view.each([&fam](const component::Relationship& rel_p1) {
                    if (rel_p1.old_id == fam.parent_1.first)
                    {
                        fam.parent_1.second = fam.parent_1.first;
                    }
                    if (rel_p1.old_id == fam.parent_2.first)
                    {
                        fam.parent_2.second = fam.parent_2.first;
                    }
                    // TODO: Solve for first time agents where parents are not in here
                });
                m_parents_reg.assign<component::Relationship>(new_entt, fam);
            }
        }
    });
}

void Relationship::add_agent(entt::entity me)
{
    auto view = m_context.registry->view<component::Relationship>();

    view.each([this, me](entt::entity e, const component::Relationship& relationship) {
        if (me == e)
        {
            // First 8 bits is friendship, last 8 bits is romance
            add_friendship(e, me, relationship.self_friend);
            add_romance(e, me, relationship.self_romance);
            return;
        }
        // TODO: for family add better relationship
        add_friendship(e, me, relationship.default_friend);
        add_romance(e, me, relationship.default_romance);
    });
}

uint8_t Relationship::get_friendship(entt::entity me, entt::entity other)
{
    return (m_rel_table[me][other] >> 8);
}
void Relationship::add_friendship(entt::entity me, entt::entity other, uint8_t amount)
{
    uint8_t friend_lvl = m_rel_table[me][other] >> 8;
    if (friend_lvl + amount > 0xFF)
    {
        m_rel_table[me][other] |= 0xFF00;
    }
    else if (friend_lvl + amount < 0)
    {
        m_rel_table[me][other] &= 0x00FF;
    }
    else
    {
        m_rel_table[me][other] += (amount << 8);
    }
}

// Get what "me" feel about the other part
uint8_t Relationship::get_romance(entt::entity me, entt::entity other)
{
    return (m_rel_table[me][other] & 0x00FF);
}

void Relationship::add_romance(entt::entity me, entt::entity other, uint8_t amount)
{
    uint8_t romance_lvl = m_rel_table[me][other] & 0x00FF;
    if (romance_lvl + amount > 0xFF)
    {
        m_rel_table[me][other] |= 0x00FF;
    }
    else if (romance_lvl + amount < 0)
    {
        m_rel_table[me][other] &= 0xFF00;
    }
    else
    {
        m_rel_table[me][other] += amount;
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
