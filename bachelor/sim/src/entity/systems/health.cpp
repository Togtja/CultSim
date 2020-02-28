#include "health.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include "spdlog/spdlog.h"

namespace cs::system
{
void Health::update(float dt)
{
    CS_AUTOTIMER(Health System);

    auto view = m_registry.view<component::Health, component::Needs, component::Tags>();
    view.each([this, dt](entt::entity e, component::Health& health, component::Needs& need, component::Tags tags) {
        for (auto& need : need.pressing_needs)
        {
            if ((need.tags & health.Need_tags) && need.status <= 0.f)
            {
                health.hp -= dt * health.tickdown_rate;
            }
        }

        if (health.hp <= 0.f)
        {
            m_registry.destroy(e);
        }
    });
}
} // namespace cs::system
