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
                repr.fertility = (1 + (age_fertile / repr.end_fertility));
            }
            else
            {
                repr.fertility = (1 - (age_fertile / repr.end_fertility));
            }
        }
        else
        {
            repr.fertility = 100.f - (repr.number_of_children * 10.f);
        }
    });

    auto preg_view = m_context.registry->view<component::Reproduction, component::Pregnancy>();
    preg_view.each([this, dt](entt::entity e, component::Reproduction& repr, component::Pregnancy& preg) {
        preg.time_since_start += dt;
        if (preg.time_since_start >= preg.gestation_period)
        {
            spawn_entity(m_context.registry, m_context.);
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
        if (preg.father == event.entity)
        {
            preg.father = entt::null;
        }
    });
}

} // namespace cs::system
