#include "requirement.h"
#include "ai/path_finding.h"
#include "common_helpers.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

namespace cs::system
{
void Requirement::update(float dt)
{
    CS_AUTOTIMER(Requirement System);

    auto& registry = *m_context.registry;
    auto view_loc  = registry.view<component::LocationRequirement, component::Movement, component::Position>();
    view_loc.each([&registry](entt::entity e,
                              component::LocationRequirement locationreqs,
                              component::Movement& mov,
                              component::Position pos) {
        if (close_enough(pos.position, locationreqs.desired_position, 5.f))
        {
            registry.remove<component::LocationRequirement>(e);
        }
        else if (mov.desired_position.empty())
        {
            ai::find_path_astar(pos.position, locationreqs.desired_position, mov.desired_position);
        }
    });

    auto view_vis = registry.view<component::VisionRequirement, component::Vision>();
    view_vis.each([&registry](entt::entity e, component::VisionRequirement visionreqs, component::Vision vision) {
        for (auto& entity : vision.seen)
        {
            if ((registry.get<component::Tags>(entity).tags & visionreqs.tags) == visionreqs.tags)
            {
                registry.remove<component::VisionRequirement>(e);
            }
        }
    });

    auto view_find = registry.view<component::FindRequirement, component::Vision, component::Position, component::Movement>();
    view_find.each([&registry, this](entt::entity e,
                                     component::FindRequirement findreqs,
                                     component::Vision vision,
                                     component::Position pos,
                                     component::Movement& mov) {
        for (auto& entity : vision.seen)
        {
            if (registry.valid(entity) && ((registry.get<component::Tags>(entity).tags & findreqs.tags) == findreqs.tags))
            {
                auto&& strat = registry.get<component::Strategies>(e);
                if (strat.staged_strategies.size() != 0)
                {
                    registry.assign<component::LocationRequirement>(e, registry.get<component::Position>(entity).position);

                    strat.staged_strategies.front().actions.back().target = entity;
                }

                registry.remove<component::FindRequirement>(e);
                mov.desired_position.clear();
                return;
            }
        }
        if (close_enough(pos.position, findreqs.desired_position, 5.f))
        {
            registry.assign_or_replace<component::FindRequirement>(
                e,
                findreqs.tags,
                glm::vec3(m_context.rng->uniform(-m_context.scenario->bounds.x, m_context.scenario->bounds.x),
                          m_context.rng->uniform(-m_context.scenario->bounds.y, m_context.scenario->bounds.y),
                          0.f));
        }
        else if (mov.desired_position.empty())
        {
            if (findreqs.desired_position == glm::vec3{0.f, 0.f, 0.f})
            {
                findreqs.desired_position =
                    glm::vec3(m_context.rng->uniform(-m_context.scenario->bounds.x, m_context.scenario->bounds.x),
                              m_context.rng->uniform(-m_context.scenario->bounds.y, m_context.scenario->bounds.y),
                              0.f);
            }
            ai::find_path_astar(pos.position, findreqs.desired_position, mov.desired_position);
        }
    });

    auto view_tag = registry.view<component::TagRequirement, component::Tags>();
    view_tag.each([this](entt::entity e, component::TagRequirement tagreqs, component::Tags& tags) {
        tags.tags = ETag(tags.tags | tagreqs.tags);
        m_context.registry->remove<component::TagRequirement>(e);
    });
}
} // namespace cs::system
