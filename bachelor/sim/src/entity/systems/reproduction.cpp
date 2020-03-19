#include "reproduction.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Reproduction::update(float dt)
{
    CS_AUTOTIMER(reproduction System);

    auto view = m_context.registry->view<component::Reproduction, component::Need>();
    view.each([dt, this](const entt::entity e, const component::Reproduction& repr, component::Need& needs) {
        if (repr.number_of_children >= repr.max_children)
        {
            ai::Need temp = {static_cast<std::string>("reproduce"), 0.f, 0.f, 0.f, ETag{}};
            auto need     = std::find(needs.needs.begin(), needs.needs.end(), temp);
            if (need != needs.needs.end())
            {
                needs.needs.erase(need);
            }
            auto pressing_need = std::find(needs.vital_needs.begin(), needs.vital_needs.end(), temp);
            if (pressing_need != needs.vital_needs.end())
            {
                needs.vital_needs.erase(pressing_need);
            }
            else
            {
                spdlog::get("agent")->info("There was an error finding the reproduction need of entity: {}", e);
            }
            m_context.registry->remove<component::Reproduction>(e);
        }
    });
}
} // namespace cs::system
