#include "movement.h"
#include "entity/components/components.h"
<<<<<<< HEAD:bachelor/sim/src/entity/systems/movement.cpp

=======
>>>>>>> 62eba4525e2695d08985798bfefdabb5a00346d7:bachelor/sim/src/entity/movement.cpp
#include <glm/glm.hpp>

namespace cs::system
{
void Movement::update(float dt)
{
    auto view = m_registry.view<component::Position, component::Movement>();
    view.each([dt](component::Position& pos, component::Movement& mov) {
        glm::vec3 temp = pos.desired_position - pos.position;
        mov.direction  = glm::normalize(glm::vec2(temp.x, temp.y));
        pos.position.x += mov.direction.x * (mov.speed * dt);
        pos.position.y += mov.direction.y * (mov.speed * dt);
    });
}
} // namespace cs::system
