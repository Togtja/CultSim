#include "reproduction.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

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
        if (auto age = m_context.registry->try_get<component::Age>(e); age)
        {
            repr.fertility = std::clamp(100.f - (age->current_age - age->max_age), 0.f, 100.f);
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
            preg.birth(e, preg.father, preg.number_of_children);
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
