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

    auto preg_view = m_context.registry->view<component::Reproduction, component::Pregnancy>();
    preg_view.each([this, dt](entt::entity e, component::Reproduction& repr, component::Pregnancy& preg) {
        preg.time_since_start += dt;
        if (preg.time_since_start >= preg.gestation_period)
        {
            const auto parent_name = m_context.registry->get<component::Meta>(e).name;

            for (int i = 0; i < preg.children_in_pregnancy; i++)
            {
                const auto new_pos = m_context.registry->get<component::Position>(e).position +
                                     glm::vec3(m_context.rng->uniform(-20.f, 20.f), m_context.rng->uniform(-20.f, 20.f), 0.f);
                auto child = spawn_entity(*m_context.registry, *m_context.lua_state, parent_name, new_pos);
                // Set age to be 0
                auto age = m_context.registry->try_get<component::Age>(child);
                if (age)
                {
                    age->current_age = 0;
                }
            }
            repr.number_of_children += preg.children_in_pregnancy;
            if (m_context.registry->valid(preg.other_parent))
            {
                auto& f_preg = m_context.registry->get<component::Reproduction>(preg.other_parent);
                f_preg.number_of_children += preg.children_in_pregnancy;
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
        if (preg.other_parent == event.entity)
        {
            preg.other_parent = entt::null;
        }
    });
}

} // namespace cs::system
