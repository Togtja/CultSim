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
    m_context.dispatcher->sink<event::EntityBorn>().connect<&Relationship::new_child_to_reg>(*this);
    m_context.dispatcher->sink<event::EntityDeleted>().connect<&Relationship::delete_father>(*this);

    sol::table cultsim = (*m_context.lua_state)["cultsim"];
    cultsim.set_function("get_friendship",
                         [this](entt::entity e, entt::entity other) -> uint8_t { return get_friendship(e, other); });
    cultsim.set_function("get_romance", [this](entt::entity e, entt::entity other) -> uint8_t { return get_romance(e, other); });

    cultsim.set_function("add_friendship", [this](entt::entity e, entt::entity other, uint8_t amount) -> void {
        add_friendship(e, other, amount);
    });

    cultsim.set_function("add_romance",
                         [this](entt::entity e, entt::entity other, uint8_t amount) -> void { add_romance(e, other, amount); });

    cultsim.set_function("get_parents", [this](entt::entity e) { return get_parents(e); });
};

void Relationship::deinitialize()
{
    m_context.dispatcher->sink<event::EntityBorn>().disconnect<&Relationship::new_child_to_reg>(*this);
    m_context.dispatcher->sink<event::EntityDeleted>().disconnect<&Relationship::delete_father>(*this);
}

entt::entity Relationship::add_to_reg(const entt::entity e)
{
    auto new_e        = m_parents_reg.create();
    const auto name_c = m_context.registry->get<component::Name>(e);
    m_parents_reg.assign<component::Name>(new_e, name_c.entity_type, name_c.name);

    auto relship_comp   = m_context.registry->get<component::Relationship>(e);
    relship_comp.old_id = e;
    m_parents_reg.assign<component::Relationship>(new_e, relship_comp);
    return new_e;
}

void Relationship::new_child_to_reg(const event::EntityBorn& event)
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
                /** Set my relationship component to have my parents relationship registry ID/entt */
                fam.mom.relationship_registry_id = e;
            }
            if (relship_comp_parent.old_id == fam.dad.global_registry_id)
            {
                fam.dad.relationship_registry_id = e;
            }
        });

        if (fam.mom.relationship_registry_id == entt::null)
        {
            /** Mom is a first gen */
            auto mom_entt                    = add_to_reg(fam.mom.global_registry_id);
            fam.mom.relationship_registry_id = mom_entt;
        }

        /** Make sure dad is still valid, could have died during pregnancy */
        if (fam.dad.relationship_registry_id == entt::null && m_context.registry->valid(fam.dad.global_registry_id))
        {
            /** Dad is a first gen */
            auto dad_entt                    = add_to_reg(fam.dad.global_registry_id);
            fam.dad.relationship_registry_id = dad_entt;
        }
        m_parents_reg.assign<component::Relationship>(new_entt, fam);
    }
}

void Relationship::delete_father(const event::EntityDeleted& event)
{
    auto view = m_context.registry->view<component::Pregnancy, component::Name, component::Relationship>();
    view.each(
        [&event,
         this](entt::entity e, const component::Pregnancy& preg, const component::Name& n, const component::Relationship& r) {
            if (preg.parents.second == event.entity)
            {
                add_to_reg(e);
            }
        });
}

void Relationship::add_relationship_table(entt::entity e)
{
    auto view = m_context.registry->view<component::Relationship>();

    auto relship_c = m_context.registry->get<component::Relationship>(e);

    view.each([this, e, relship_c](entt::entity e2, const component::Relationship& relationship) {
        if (e == e2)
        {
            /** First 8 bits is friendship, last 8 bits is romance */
            add_friendship(e, e2, relationship.self_friend);
            add_romance(e, e2, relationship.self_romance);
            return;
        }
        /** TODO: for family add better relationship */
        /** What I feel for others */
        add_friendship(e, e2, relship_c.default_friend);
        add_romance(e, e2, relship_c.default_romance);

        /** What others feel for me */
        add_friendship(e2, e, relationship.default_friend);
        add_romance(e2, e, relationship.default_romance);
    });
}

uint8_t Relationship::get_friendship(entt::entity e, entt::entity other)
{
    return (m_rel_table[e][other] >> 8);
}

void Relationship::add_friendship(entt::entity e, entt::entity other, uint8_t amount)
{
    uint8_t friend_lvl = m_rel_table[e][other] >> 8;
    if (friend_lvl + amount > 0xFF)
    {
        m_rel_table[e][other] |= 0xFF00;
    }
    else if (friend_lvl + amount < 0)
    {
        m_rel_table[e][other] &= 0x00FF;
    }
    else
    {
        m_rel_table[e][other] += (amount << 8);
    }
}

uint8_t Relationship::get_romance(entt::entity e, entt::entity other)
{
    return (m_rel_table[e][other] & 0x00FF);
}

void Relationship::add_romance(entt::entity e, entt::entity other, uint8_t amount)
{
    uint8_t romance_lvl = m_rel_table[e][other] & 0x00FF;
    if (romance_lvl + amount > 0xFF)
    {
        m_rel_table[e][other] |= 0x00FF;
    }
    else if (romance_lvl + amount < 0)
    {
        m_rel_table[e][other] &= 0xFF00;
    }
    else
    {
        m_rel_table[e][other] += amount;
    }
}

BothParentName Relationship::get_parents(entt::entity e, bool is_local_ids)
{
    BothParentName ret;
    if (is_local_ids)
    {
        const auto& rel = m_parents_reg.get<component::Relationship>(e);
        auto par1_name  = m_parents_reg.get<component::Name>(rel.mom.relationship_registry_id);
        auto par2_name  = m_parents_reg.get<component::Name>(rel.dad.relationship_registry_id);
        ret.mom.name    = par1_name.name;
        ret.dad.name    = par2_name.name;
        ret.mom.ids     = rel.mom;
        ret.dad.ids     = rel.dad;
        return ret;
    }

    auto view = m_parents_reg.view<component::Relationship>();
    for (auto ent : view)
    {
        const auto& rel = m_parents_reg.get<component::Relationship>(ent);
        if (rel.old_id == e)
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

            ret.mom.name = par1_name.name;
            ret.dad.name = par2_name.name;
            ret.mom.ids  = rel.mom;
            ret.dad.ids  = rel.dad;
            return ret;
        }
    }
    return ret;
}

/** TODO: Replace with registry on construct event to remove need for per frame runs*/
void Relationship::update(float dt)
{
    CS_AUTOTIMER(Relationship System);
    auto view = m_context.registry->view<component::Relationship>();
    view.each([this, &view](entt::entity e, component::Relationship& relationship) {
        if (relationship.new_create)
        {
            add_relationship_table(e);
            relationship.new_create = false;
        }
    });
}

ISystem* Relationship::clone()
{
    return new Relationship(m_context);
}
} // namespace cs::system
