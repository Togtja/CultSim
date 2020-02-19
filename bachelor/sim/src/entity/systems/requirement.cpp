#include "requirement.h"
#include "entity/components/components.h"
#include "glm/glm.hpp"

namespace cs::system
{
void Requirement::update(float dt)
{
    auto view = m_registry.view<component::LocationRequirement, component::Position>();
    view.each([this, dt](entt::entity e, component::LocationRequirement locationreqs, component::Position pos) {
        if (pos.position == locationreqs.desired_position)
        {
            m_registry.remove<component::LocationRequirement>(e);
        }
        else if (pos.desired_position != locationreqs.desired_position)
        {
            pos.desired_position = locationreqs.desired_position;
        }
    });
}
} // namespace cs::system