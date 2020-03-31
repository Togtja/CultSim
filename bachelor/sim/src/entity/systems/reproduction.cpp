#include "reproduction.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
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

    auto preg_view = m_context.registry->group<>(entt::get<component::Reproduction, component::Pregnancy>);
    preg_view.each([this, dt](entt::entity e, component::Reproduction& repr, component::Pregnancy& preg) {
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
                    auto egg           = spawn_entity(*m_context.registry, *m_context.lua_state, repr.egg_type, new_pos);
                    auto& egg_time     = m_context.registry->get<component::Reproduction>(egg);
                    auto& egg_hatching = m_context.registry->assign_or_replace<component::Pregnancy>(egg);

                    egg_time.lays_eggs = false;

                    egg_hatching.children_in_pregnancy = 1;
                    egg_hatching.parents[0]            = e;
                    egg_hatching.parents[1]            = preg.parents[0];
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
                    m_context.registry->get<component::Meta>(egg).name = parent_name;
                }
                else
                {
                    entt::entity child = entt::null;
                    if (preg.is_egg)
                    {
                        auto egg_pos = m_context.registry->get<component::Position>(e).position;
                        child        = spawn_entity(*m_context.registry, *m_context.lua_state, parent_name, egg_pos);
                    }
                    else
                    {
                        child = spawn_entity(*m_context.registry, *m_context.lua_state, parent_name, new_pos);
                    }
                    // Set age to be 0
                    auto age = m_context.registry->try_get<component::Age>(child);
                    if (age)
                    {
                        age->current_age = 0;
                    }
                }
            }
            repr.number_of_children += preg.children_in_pregnancy;
            if (m_context.registry->valid(preg.parents[1]))
            {
                auto& f_preg = m_context.registry->get<component::Reproduction>(preg.parents[1]);
                f_preg.number_of_children += preg.children_in_pregnancy;
            }
            if (preg.is_egg)
            {
                m_context.registry->assign_or_replace<component::Delete>(e);
            }
            m_context.registry->remove<component::Pregnancy>(e);
        }
    });
}

ISystem* Reproduction::clone()
{
    return new Reproduction(m_context);
}

void Reproduction::delete_father(const event::DeleteEntity& event)
{
    auto view = m_context.registry->view<component::Pregnancy>();
    view.each([&event](component::Pregnancy& preg) {
        if (preg.parents[1] == event.entity)
        {
            preg.parents[1] = entt::null;
        }
    });
}

} // namespace cs::system
