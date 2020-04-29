#include "reproduction.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/effect.h"
#include "entity/factory.h"

namespace cs::system
{
void Reproduction::initialize()
{
    m_context.dispatcher->sink<event::DeleteEntity>().connect<&Reproduction::delete_father>(*this);
}

void Reproduction::deinitialize()
{
    m_context.dispatcher->sink<event::DeleteEntity>().disconnect<&Reproduction::delete_father>(*this);
}
void Reproduction::update(float dt)
{
    CS_AUTOTIMER(Reproduction System);

    auto view = m_context.registry->view<component::Reproduction>();
    view.each([dt, this](const entt::entity e, component::Reproduction& repr) {
        if (auto age = m_context.registry->try_get<component::Age>(e); age && repr.has_fertility)
        {
            // Can not reproduce after end of fertility
            if (age->current_age > repr.end_fertility || age->current_age < repr.start_fertility)
            {
                repr.fertility = 0;
                return;
            }
            auto age_fertile = age->current_age - repr.peak_fertility;
            if (age_fertile < 0)
            {
                repr.fertility = (1.f + (age_fertile / repr.end_fertility));
            }
            else
            {
                repr.fertility = (1.f - (age_fertile / repr.end_fertility));
            }
        }
        if (repr.fertility < 0)
        {
            repr.fertility = 0;
        }
    });
    // Children that will be spawned
    std::vector<Child> children;

    auto preg_view = m_context.registry->view<component::Reproduction, component::Pregnancy>();
    preg_view.each([this, dt, &preg_view, &children](entt::entity e, component::Reproduction& repr, component::Pregnancy& preg) {
        preg.time_since_start += dt;

        if (preg.time_since_start >= preg.gestation_period)
        {
            const auto parent_name = m_context.registry->get<component::Meta>(e).name;

            for (int i = 0; i < preg.children_in_pregnancy; i++)
            {
                const auto new_pos = m_context.registry->get<component::Position>(e).position +
                                     glm::vec3(m_context.rng->uniform(-20.f, 20.f), m_context.rng->uniform(-20.f, 20.f), 0.f);
                if (repr.lays_eggs)
                {
                    children.push_back(
                        Child{repr.egg_type, new_pos, true, parent_name, {preg.parents.first, preg.parents.second}});
                }
                else
                {
                    if (preg.is_egg)
                    {
                        auto egg_pos = m_context.registry->get<component::Position>(e).position;
                        children.push_back(
                            Child{parent_name, egg_pos, false, parent_name, {preg.parents.first, preg.parents.second}});
                    }
                    else
                    {
                        children.push_back(Child{parent_name, new_pos, false, parent_name, preg.parents});
                    }
                }
            }
            repr.number_of_children += preg.children_in_pregnancy;
            if (!preg.is_egg && m_context.registry->valid(preg.parents.second))
            {
                auto& f_preg = m_context.registry->get<component::Reproduction>(preg.parents.second);
                f_preg.number_of_children += preg.children_in_pregnancy;
            }
            if (preg.is_egg)
            {
                m_context.registry->assign_or_replace<component::Delete>(e);
            }
            m_context.registry->remove<component::Pregnancy>(e);
        }
    });

    for (auto&& child : children)
    {
        auto child_e = spawn_entity(*m_context.registry, *m_context.lua_state, child.type, child.position);

        if (child.is_egg)
        {
            auto& egg_time     = m_context.registry->get<component::Reproduction>(child_e);
            auto& egg_hatching = m_context.registry->assign_or_replace<component::Pregnancy>(child_e);
            egg_time.lays_eggs = false;

            egg_hatching.children_in_pregnancy = 1;
            egg_hatching.parents.first         = child.parents.first;
            egg_hatching.parents.second        = child.parents.second;
            egg_hatching.is_egg                = true;

            if (egg_time.gestation_deviation > 0)
            {
                egg_hatching.gestation_period =
                    std::round(m_context.rng->normal(egg_time.average_gestation_period, egg_time.gestation_deviation));
            }
            else
            {
                egg_hatching.gestation_period = egg_time.average_gestation_period;
            }
            // Makes sure the eggs spawn the parent type
            m_context.registry->get<component::Meta>(child_e).name = child.parent_type;
            return;
        }
        m_context.dispatcher->enqueue<event::BornEntity>(event::BornEntity{child_e});
        // TODO: Figure out if this is redundant
        if (auto age = m_context.registry->try_get<component::Age>(child_e); age)
        {
            age->current_age = 0;
        }

        if (auto traits = m_context.registry->try_get<component::Traits>(child_e); traits)
        {
            // Check if we can mutate something
            for (auto&& i : traits->attainable_traits)
            {
                if (i.can_mutate && m_context.rng->trigger(i.mutate_chance))
                {
                    traits->acquired_traits.push_back(i);
                }
            }
            // Tries to inherit mom's acquired traits
            if (m_context.registry->valid(child.parents.first))
            {
                if (auto mom_traits = m_context.registry->try_get<component::Traits>(child.parents.first); mom_traits)
                {
                    for (auto&& i : mom_traits->acquired_traits)
                    {
                        if (i.can_inherit && m_context.rng->trigger(i.inherit_chance))
                        {
                            traits->acquired_traits.push_back(i);
                        }
                    }
                }
            }

            if (m_context.registry->valid(child.parents.second))
            {
                // Tries to inherit dad's acquired traits
                if (auto dad_traits = m_context.registry->try_get<component::Traits>(child.parents.second); dad_traits)
                {
                    for (auto&& i : dad_traits->acquired_traits)
                    {
                        if (i.can_inherit && m_context.rng->trigger(i.inherit_chance))
                        {
                            traits->acquired_traits.push_back(i);
                        }
                    }
                }
            }
            // Make our list unique
            auto it = std::unique(traits->acquired_traits.begin(), traits->acquired_traits.end());
            traits->acquired_traits.resize(std::distance(traits->acquired_traits.begin(), it));
            // Run the effect of the acquired traits
            effect::affect_traits(child_e, *traits);
        }
        if (auto rel_c = m_context.registry->try_get<component::Relationship>(child_e); rel_c)
        {
            if (m_context.registry->valid(child.parents.first))
            {
                rel_c->mom.global_registry_id = child.parents.first;
            }
            if (m_context.registry->valid(child.parents.second))
            {
                rel_c->dad.global_registry_id = child.parents.second;
            }
        }
    }
} // namespace cs::system

ISystem* Reproduction::clone()
{
    return new Reproduction(m_context);
}

void Reproduction::delete_father(const event::DeleteEntity& event)
{
    auto view = m_context.registry->view<component::Pregnancy>();
    view.each([&event](component::Pregnancy& preg) {
        if (preg.parents.first == event.entity)
        {
            // preg.parents.second = entt::null;
        }
    });
}

} // namespace cs::system
