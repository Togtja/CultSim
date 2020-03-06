#include "health.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include "spdlog/spdlog.h"

namespace cs::system
{
void Health::update(float dt)
{
    CS_AUTOTIMER(Health System);

    auto view = m_context.registry->view<component::Health, component::Needs, component::Tags>();
    view.each([this, dt](entt::entity e, component::Health& health, component::Needs& need, component::Tags tags) {
        for (auto& need : need.vital_needs)
        {
            if ((need.tags & health.need_tags) && need.status <= 0.f)
            {
                health.hp -= dt * health.tickdown_rate * need.vitality;
                if (health.hp <= 0.f)
                {
                    spdlog::warn("Entity {} killed by need {}", e, need.name);
                }
            }
        }

        if (health.hp <= 0.f)
        {
            m_context.registry->destroy(e);
        }
        health.hp = std::clamp(health.hp + dt * health.tickdown_rate * 0.1f, 0.f, 100.f);
    });
}
} // namespace cs::system
