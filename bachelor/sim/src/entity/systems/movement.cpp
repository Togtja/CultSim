#include "movement.h"
#include "entity/components/components.h"

#include <execution>
#include <random>

#include <glm/glm.hpp>

namespace cs::system
{
void Movement::update(float dt)
{
    static auto seed = std::random_device{};
    static auto gen  = std::mt19937{seed()};
    std::normal_distribution<float> rng(0.f, 1.f);

    auto view = m_registry.group<component::Movement, component::Position>();
    view.each([dt, &rng](component::Movement& mov, component::Position& pos) {
        glm::vec3 temp = pos.desired_position - pos.position;
        mov.direction  = glm::normalize(temp);
        pos.position += glm::vec3(mov.direction * mov.avoidance * (mov.speed * dt), 0.f);
        if (glm::distance(pos.position, pos.desired_position) < 10.f)
        {
            pos.desired_position = {rng(seed) * 15000.f, rng(seed) * 15000.f, 0.f};
        }
        mov.avoidance = glm::vec3(1); // Reset the avoidance force
    });
}
} // namespace cs::system
