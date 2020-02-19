#include "requirement.h"
#include "entity/components/components.h"
#include "common_helpers.h"

#include "glm/glm.hpp"
#include "spdlog/spdlog.h"


namespace cs::system
{
void Requirement::update(float dt)
{
    auto view = m_registry.view<component::LocationRequirement, component::Position>();
    view.each([this, dt](entt::entity e, component::LocationRequirement& locationreqs, component::Position& pos) {
        if (close_enough(pos.position,locationreqs.desired_position, 10.f))
        {
            spdlog::warn("We have arrived, entity: {}",static_cast<uint32_t>(e));
            m_registry.remove<component::LocationRequirement>(e);
        }
        else if (pos.desired_position != locationreqs.desired_position)
        {
            pos.desired_position = locationreqs.desired_position;
        }
    });
}
} // namespace cs::system