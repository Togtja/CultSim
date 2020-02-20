#include "requirement.h"
#include "common_helpers.h"
#include "entity/components/components.h"
#include "ai/path_finding.h"

#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

namespace cs::system
{
void Requirement::update(float dt)
{
    auto view_loc = m_registry.view<component::LocationRequirement, component::Movement, component::Position>();
    view_loc.each([this, dt](entt::entity e, component::LocationRequirement locationreqs, component::Movement& mov, component::Position pos) {
        if (close_enough(pos.position, locationreqs.desired_position, 10.f))
        {
            spdlog::warn("We have arrived, entity: {}", static_cast<uint32_t>(e));
            m_registry.remove<component::LocationRequirement>(e);
        }
        else if (mov.desired_position.empty())
        {
            ai::path_finding(pos.position,locationreqs.desired_position,mov.desired_position);
        }
    });

    auto view_vis = m_registry.view<component::VisionRequirement, component::Vision>();
    view_vis.each([this, dt](entt::entity e, component::VisionRequirement visionreqs, component::Vision vision) {
        for (auto& entity : vision.seen)
        {
            if ((m_registry.get<component::Tags>(entity).tags & visionreqs.tags) == visionreqs.tags)
            {
                m_registry.remove<component::VisionRequirement>(e);
            }
        }
    });

    auto view_find = m_registry.view<component::FindRequirement, component::Vision, component::Position, component::Movement>();
    view_find.each([this,
                    dt](entt::entity e, component::FindRequirement findreqs, component::Vision vision, component::Position pos, component::Movement& mov) {
        for (auto& entity : vision.seen)
        {
            if ((m_registry.get<component::Tags>(entity).tags & findreqs.tags) == findreqs.tags)
            {
                m_registry.assign<component::LocationRequirement>(e, m_registry.get<component::Position>(entity).position);
                m_registry.remove<component::FindRequirement>(e);
                return;
            }
        }
        if (close_enough(pos.position, findreqs.desired_position,10.f))
        {
            m_registry.assign_or_replace<component::FindRequirement>(e, findreqs.tags, glm::vec3(m_rng.uniform(-500.f, 500.f)));
        }
        else if (mov.desired_position.empty())
         {
            ai::path_finding(pos.position,findreqs.desired_position,mov.desired_position);
        }
    });
}
} // namespace cs::system