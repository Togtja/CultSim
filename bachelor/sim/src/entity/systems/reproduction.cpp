#include "reproduction.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Reproduction::update(float dt)
{
    CS_AUTOTIMER(Reproduction System);

    auto view = m_context.registry->view<component::Reproduction, component::Need>();
    view.each([dt, this](const entt::entity e, component::Reproduction& repr, component::Need& needs) {
        if (auto age = m_context.registry->try_get<component::Age>(e); age)
        {
            std::clamp(repr.fertility = 100.f - (age->current_age - age->max_age), 0.f, 100.f);
        }
        else
        {
            repr.fertility = 100.f - (repr.number_of_children * 10.f);
        }
    });
}
} // namespace cs::system
