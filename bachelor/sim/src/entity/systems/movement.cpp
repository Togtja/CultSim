#include "movement.h"
#include "ai/path_finding.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/events.h"

#include <execution>
#include <random>

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace cs::system
{
Movement::Movement(entt::registry& registry, entt::dispatcher& dispatcher) : ISystem(registry), m_dispatcher(dispatcher)
{
}

void Movement::update(float dt)
{
    CS_AUTOTIMER(Movement System);

    static auto seed = std::random_device{};
    static auto gen  = std::mt19937{seed()};
    std::uniform_real_distribution rng(-1.f, 1.f);
    auto view = m_registry.view<component::Movement, component::Position>();
    view.each([dt, &rng, this](entt::entity e, component::Movement& mov, component::Position& pos) {
        if (mov.desired_position.empty())
        {
            return;
        }
        auto cur_head  = mov.desired_position.back();
        glm::vec3 temp = cur_head - pos.position;

        if (glm::length(temp) != 0)
        {
            mov.direction = glm::normalize(temp);
        }
        else
        {
            mov.direction = temp;
        }
        pos.position += glm::vec3(mov.direction * (mov.speed * dt), 0.f);

        if (glm::distance(pos.position, cur_head) < 5.f)
        {
            mov.desired_position.pop_back();
            if (mov.desired_position.empty())
            {
                // Arrived at final destination
                m_dispatcher.enqueue(event::ArrivedAtDestination{e, cur_head});
            }
        }
    });
}
} // namespace cs::system
