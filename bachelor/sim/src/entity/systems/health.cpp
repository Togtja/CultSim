#include "health.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/events.h"

#include <spdlog/spdlog.h>

namespace cs::system
{
void Health::update(float dt)
{
    CS_AUTOTIMER(Health System);

    auto view = m_context.registry->view<component::Health, component::Need>();
    view.each([this, dt](entt::entity e, component::Health& health, component::Need& need) {
        for (auto& need : need.vital_needs)
        {
            if ((need.tags & health.need_tags) && need.status <= 0.f)
            {
                health.health -= dt * health.tickdown_rate * need.vitality;
                if (health.health <= 0.f)
                {
                    spdlog::get("agent")->info("Entity {} killed by need {}", e, need.name);
                }
            }
        }
        if (health.health <= 0.f)
        {
            m_context.registry->assign<component::Delete>(e);
            auto tags = m_context.registry->try_get<component::Tags>(e);
            if (tags)
            {
                tags->tags = static_cast<ETag>(tags->tags | ETag::TAG_Delete);
            }
            return;
        }

        if (auto age = m_context.registry->try_get<component::Age>(e); age)
        {
            age->current_age += dt;
            /** +0.01 just to avoid crashes for max_age == 0*/
            float age_difference   = age->current_age / age->max_age + 0.01;
            float health_reduction = std::clamp(0 + 100 * age_difference, 0.f, 100.f);

            health.health = std::clamp(health.health + dt * health.tickdown_rate * 0.1f, 0.f, 100 - health_reduction);
        }
        else
        {
            health.health = std::clamp(health.health + dt * health.tickdown_rate * 0.1f, 0.f, 100.f);
        }
    });
}

ISystem* Health::clone()
{
    return new Health(m_context);
}
} // namespace cs::system
