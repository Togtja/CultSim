#include "reproduction.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Reproduction::update(float dt)
{
    CS_AUTOTIMER(Reproduction System);

    auto view = m_registry.view<component::Reproduction, component::Needs, component::Tags>();
    view.each([dt, this](entt::entity e, component::Reproduction& repr, component::Needs& needs, component::Tags& tags) {
        if (repr.number_of_children > repr.max_children)
        {
            ai::Need need = {static_cast<std::string>("reproduce"), 0.f, 0.f, 0.f, ETag{}};
            needs.needs.erase(std::find(needs.needs.begin(), needs.needs.end(), need));
            needs.pressing_needs.erase(std::find(needs.pressing_needs.begin(), needs.pressing_needs.end(), need));

            m_registry.remove<component::Reproduction>(e);
        }
    });
}
} // namespace cs::system
