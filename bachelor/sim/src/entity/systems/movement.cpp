#include "movement.h"
#include "entity/components/components.h"
#include "entity/events.h"

#include <random>

#include <random>

#include <glm/glm.hpp>

namespace cs::system
{
Movement::Movement(entt::registry& registry, entt::dispatcher& dispatcher) : ISystem(registry), m_dispatcher(dispatcher)
{
}

void Movement::update(float dt)
{
    static auto seed = std::random_device{};
    static auto gen  = std::mt19937{seed()};
    std::normal_distribution<float> rng(0.f, 1.f);

    auto view = m_registry.view<component::Position, component::Movement>();
    view.each([dt, &rng, this](entt::entity e, component::Position& pos, component::Movement& mov) {
        glm::vec3 temp = pos.desired_position - pos.position;
        mov.direction  = glm::normalize(temp);
        pos.position += glm::vec3(mov.direction * (mov.speed * dt), 0.f);

        if (glm::distance(pos.position, pos.desired_position) < 10.f)
        {
            m_dispatcher.enqueue(event::ArrivedAtDestination{e, pos.desired_position});
        }
    });
}
} // namespace cs::system
