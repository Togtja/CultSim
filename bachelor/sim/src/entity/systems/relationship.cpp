#include "relationship.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include <sol/sol.hpp>

namespace cs::system
{
Relationship::Relationship(const Relationship& other) : ISystem(other), m_rel_table(other.m_rel_table)
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
    m_context.dispatcher->sink<event::EntitySpawned>().connect<&Relationship::add_relationship_table>(*this);

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

    cultsim.set_function("get_children", [this](entt::entity e) { return get_children(e); });

    cultsim.set_function("is_family", [this](entt::entity e, entt::entity other) -> bool { is_family(e, other); });

    cultsim.set_function("is_family_gen",
                         [this](entt::entity e, entt::entity other, unsigned gen) -> bool { is_family(e, other, gen); });
};

void Relationship::deinitialize()
{
    m_context.dispatcher->sink<event::EntityBorn>().disconnect<&Relationship::new_child_to_reg>(*this);
    m_context.dispatcher->sink<event::EntitySpawned>().disconnect<&Relationship::add_relationship_table>(*this);
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
    if (relship_comp.mom.global != entt::null && relship_comp.dad.global != entt::null)
    {
        component::Relationship fam{};
        fam.old_id     = event.new_born;
        fam.mom.global = relship_comp.mom.global;
        fam.dad.global = relship_comp.dad.global;

        auto view = m_parents_reg.view<component::Relationship>();
        view.each([&fam](entt::entity e, const component::Relationship& relship_comp_parent) {
            if (relship_comp_parent.old_id == fam.mom.global)
            {
                /** Set my relationship component to have my parents relationship registry ID/entt */
                fam.mom.relationship = e;
            }
            if (relship_comp_parent.old_id == fam.dad.global)
            {
                fam.dad.relationship = e;
            }
        });

        if (fam.mom.relationship == entt::null)
        {
            /** Mom is a first gen */
            auto mom_entt        = add_to_reg(fam.mom.global);
            fam.mom.relationship = mom_entt;
        }

        /** Make sure dad is still valid, could have died during pregnancy */
        if (fam.dad.relationship == entt::null && m_context.registry->valid(fam.dad.global))
        {
            /** Dad is a first gen */
            auto dad_entt        = add_to_reg(fam.dad.global);
            fam.dad.relationship = dad_entt;
        }
        m_parents_reg.assign<component::Relationship>(new_entt, fam);

        /** Add me as a child to my parents*/
        auto& mom_rel = m_parents_reg.get<component::Relationship>(fam.mom.relationship);
        mom_rel.children.push_back({fam.old_id, new_entt});

        auto& dad_rel = m_parents_reg.get<component::Relationship>(fam.dad.relationship);
        dad_rel.children.push_back({fam.old_id, new_entt});
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

void Relationship::add_relationship_table(const event::EntitySpawned& event)
{
    if (auto relship_c = m_context.registry->try_get<component::Relationship>(event.entity); relship_c)
    {
        auto view = m_context.registry->view<component::Relationship>();
        view.each([this, event, relship_c](entt::entity e2, const component::Relationship& relationship) {
            if (event.entity == e2)
            {
                /** First 8 bits is friendship, last 8 bits is romance */
                add_friendship(event.entity, e2, relationship.self_friend);
                add_romance(event.entity, e2, relationship.self_romance);
                return;
            }
            /** TODO: for family add better relationship */
            if (is_family(event.entity, e2)) {}
            /** What I feel for others */
            add_friendship(event.entity, e2, relship_c->default_friend);
            add_romance(event.entity, e2, relship_c->default_romance);

            /** What others feel for me */
            add_friendship(e2, event.entity, relationship.default_friend);
            add_romance(e2, event.entity, relationship.default_romance);
        });
    }
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

ParentsName Relationship::get_parents(entt::entity e, bool is_local_id)
{
    ParentsName ret;
    if (is_local_id)
    {
        const auto& rel = m_parents_reg.get<component::Relationship>(e);
        auto par1_name  = m_parents_reg.get<component::Name>(rel.mom.relationship);
        auto par2_name  = m_parents_reg.get<component::Name>(rel.dad.relationship);
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
            auto& par1_name = m_parents_reg.get<component::Name>(rel.mom.relationship);
            auto& par2_name = m_parents_reg.get<component::Name>(rel.dad.relationship);

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

    spdlog::get("agent")->info("could not find the specified entity {} when getting parents", e);
    return ret;
}

std::vector<EntityNameAndIds> Relationship::get_children(entt::entity e, bool is_local_id)
{
    std::vector<EntityNameAndIds> ret{};
    if (is_local_id)
    {
        const auto& rel = m_parents_reg.get<component::Relationship>(e);
        for (const auto& child : rel.children)
        {
            EntityNameAndIds add_child;
            add_child.ids  = child;
            add_child.name = m_parents_reg.get<component::Name>(child.relationship).name;
            ret.push_back(add_child);
        }

        return ret;
    }

    auto view = m_parents_reg.view<component::Relationship>();
    for (auto ent : view)
    {
        const auto& rel = m_parents_reg.get<component::Relationship>(ent);
        if (rel.old_id == e)
        {
            for (const auto& child : rel.children)
            {
                EntityNameAndIds add_child;
                add_child.ids  = child;
                add_child.name = m_parents_reg.get<component::Name>(child.relationship).name;
                ret.push_back(add_child);
            }
            return ret;
        }
    }
    spdlog::get("agent")->info("could not find the specified entity {} when getting children", e);
    return ret;
}

bool Relationship::is_family(entt::entity me, entt::entity other, unsigned gen, bool is_local_id)
{
    if (me == other)
    {
        spdlog::get("agent")->warn("checking if self is family with yourself, always true");
        return true;
    }
    std::vector<EntityNameAndIds> to_check;

    /** Add children and parents to the to_check */
    to_check         = get_children(me, is_local_id);
    const auto rel_e = get_parents(me, is_local_id);
    to_check.emplace_back(rel_e.dad);
    to_check.emplace_back(rel_e.mom);

    while (!to_check.empty())
    {
        for (const auto& maybe_family : to_check)
        {
            if (maybe_family.ids.global == other)
            {
                return true;
            }
        }

        /** Add more family to check */
        if (gen > 0)
        {
            std::vector<cs::system::EntityNameAndIds> add{};
            for (const auto& maybe_family : to_check)
            {
                /** The family member does not exist, happens when parents are the simulation */
                if (maybe_family.ids.relationship == entt::null)
                {
                    continue;
                }
                auto children = get_children(maybe_family.ids.relationship, true);
                auto parents  = get_parents(maybe_family.ids.relationship, true);

                add.insert(add.end(), children.begin(), children.end());
                add.push_back(parents.mom);
                add.push_back(parents.dad);
            }
            /** Clear the already checked members*/
            to_check.clear();
            gen--; // Subtract from generation
            /** Insert the new potential members to check */
            to_check.insert(to_check.end(), add.begin(), add.end());
        }
        else
        {
            to_check.clear();
        }
    }
    return false;
}

bool Relationship::is_family_local(entt::entity me, entt::entity other, unsigned gen)
{
    return false;
}

void Relationship::update(float dt)
{
}

ISystem* Relationship::clone()
{
    return new Relationship(m_context);
}
} // namespace cs::system
