#include "movement.h"
#include "components.h"

#include <random>

#include <glm/glm.hpp>

namespace cs::system
{
void Movement::update(float dt)
{
    static auto seed = std::random_device{};
    static auto gen = std::mt19937{seed()};
    std::uniform_real_distribution<float> rng(-1.f, 1.f);

    auto view = m_registry.view<component::Position, component::Movement>();
    view.each([dt, &rng](component::Position& pos, component::Movement& mov) {
        glm::vec3 temp = pos.desired_position - pos.position;
        mov.direction  = glm::normalize(temp);
        pos.position += glm::vec3(mov.direction * (mov.speed * dt), 0.f);

        if(glm::distance(pos.position, pos.desired_position) < 10.f)
        {
            pos.desired_position = {rng(seed) * 500.f, rng(seed) * 500.f, 0.f};
        }
    });
}
} // namespace cs::system
