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

    const auto name_c = m_context.registry->get<component::Name>(event.new_born);
    m_parents_reg.assign<component::Name>(new_entt, name_c.entity_type, name_c.name);

    auto relship_comp = m_context.registry->get<component::Relationship>(event.new_born);
    if (relship_comp.mom.global_registry_id != entt::null && relship_comp.dad.global_registry_id != entt::null)
    {
        component::Relationship fam{};
        fam.old_id                 = event.new_born;
        fam.mom.global_registry_id = relship_comp.mom.global_registry_id;
        fam.dad.global_registry_id = relship_comp.dad.global_registry_id;

        auto view = m_parents_reg.view<component::Relationship>();
        view.each([&fam](entt::entity e, const component::Relationship& relship_comp_parent) {
            if (relship_comp_parent.old_id == fam.mom.global_registry_id)
            {
                // Set my relationship component to have my parents relationship registry ID/entt
                fam.mom.relationship_registry_id = e;
            }
            if (relship_comp_parent.old_id == fam.dad.global_registry_id)
            {
                fam.dad.relationship_registry_id = e;
            }
        });
        if (fam.mom.relationship_registry_id == entt::null)
        {
            // Mom is a first gen
            auto mom_entt         = m_parents_reg.create();
            const auto mom_name_c = m_context.registry->get<component::Name>(fam.mom.global_registry_id);
            m_parents_reg.assign<component::Name>(mom_entt, mom_name_c.entity_type, mom_name_c.name);

            auto mom_relship_comp = m_context.registry->get<component::Relationship>(fam.mom.global_registry_id);
            m_parents_reg.assign<component::Relationship>(mom_entt, mom_relship_comp);
            fam.mom.relationship_registry_id = mom_entt;
        }
        if (fam.dad.relationship_registry_id == entt::null)
        {
            // Dad is a first gen
            auto dad_entt         = m_parents_reg.create();
            const auto dad_name_c = m_context.registry->get<component::Name>(fam.dad.global_registry_id);
            m_parents_reg.assign<component::Name>(dad_entt, dad_name_c.entity_type, dad_name_c.name);

            auto dad_relship_comp = m_context.registry->get<component::Relationship>(fam.dad.global_registry_id);
            m_parents_reg.assign<component::Relationship>(dad_entt, dad_relship_comp);
            fam.dad.relationship_registry_id = dad_entt;
        }
        m_parents_reg.assign<component::Relationship>(new_entt, fam);
    }
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

BothParentName Relationship::get_parent(entt::entity me, bool is_regrel_nr)
{
    if (is_regrel_nr)
    {
        const auto& rel = m_parents_reg.get<component::Relationship>(me);
        auto par1_name  = m_parents_reg.get<component::Name>(rel.mom.relationship_registry_id);
        auto par2_name  = m_parents_reg.get<component::Name>(rel.dad.relationship_registry_id);
        return {{rel.mom.relationship_registry_id, par1_name.name}, {rel.dad.relationship_registry_id, par2_name.name}};
    }
    auto view = m_parents_reg.view<component::Relationship>();
    for (auto&& ent : view)
    {
        const auto& rel = m_parents_reg.get<component::Relationship>(ent);
        if (rel.old_id == me)
        {
            if (m_parents_reg.valid(rel.mom.relationship_registry_id) && m_parents_reg.valid(rel.dad.relationship_registry_id))
            {
                auto& par1_name = m_parents_reg.get<component::Name>(rel.mom.relationship_registry_id);
                auto& par2_name = m_parents_reg.get<component::Name>(rel.dad.relationship_registry_id);
                if (par1_name.name.empty())
                {
                    par1_name.name = par1_name.entity_type;
                }
                if (par2_name.name.empty())
                {
                    par2_name.name = par2_name.entity_type;
                }
                return {{rel.mom.relationship_registry_id, par1_name.name}, {rel.dad.relationship_registry_id, par2_name.name}};
            }
            else
            {
                // The parents are First spawners
            }
        }
    }
    return {{entt::null, ""}, {entt::null, ""}};
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
