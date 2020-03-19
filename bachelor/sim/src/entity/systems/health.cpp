#include "health.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include "spdlog/spdlog.h"

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
                    spdlog::get("agent")->warn("Entity {} killed by need {}", e, need.name);
                }
            }
        }

        // TODO: Better entity removal upon death
        if (health.health <= 0.f)
        {
            m_context.registry->destroy(e);
        }
        
        if (auto age = m_context.registry->try_get<component::Age>(e); age)
        {
            age->current_age += dt;

            auto age_above_max = age->current_age - age->max_age;
            health.health =
                std::clamp(health.health + dt * health.tickdown_rate * 0.1f, 0.f, 100.f - (age->max_age - age->current_age));
        }
        else
        {
            health.health = std::clamp(health.health + dt * health.tickdown_rate * 0.1f, 0.f, 100.f);
        }

    });
}
} // namespace cs::system
