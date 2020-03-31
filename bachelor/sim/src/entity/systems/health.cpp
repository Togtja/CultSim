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
            m_timer += dt;
            age->current_age += dt;
            if (m_timer > 1.f)
            {

                m_timer           = 0.f;
                auto death_chance = pow(sigmoid(pow(age->current_age,2) / pow(age->average_life_expectancy,2)),20);
                auto death_roll   = m_context.rng->trigger(death_chance);
                if (death_roll && dt != 0)
                {
                    spdlog::get("agent")->warn(
                        "We killed an entity due to old age, death chance: {} , current age : {}, average age: {}",
                        death_chance,
                        age->current_age,
                        age->average_life_expectancy);
                    m_context.registry->assign<component::Delete>(e);
                    auto tags = m_context.registry->try_get<component::Tags>(e);
                    if (tags)
                    {
                        tags->tags = static_cast<ETag>(tags->tags | ETag::TAG_Delete);
                    }
                    return;
                }
            }
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
