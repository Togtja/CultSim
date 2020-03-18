#include "requirement.h"
#include "ai/path_finding.h"
#include "common_helpers.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/events.h"
#include "entity/memories/resource_location.h"

#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

namespace cs::system
{
void Requirement::update(float dt)
{
    CS_AUTOTIMER(Requirement System);

    auto& registry   = *m_context.registry;
    auto& dispatcher = *m_context.dispatcher;

    auto view_loc = registry.view<component::LocationRequirement, component::Movement, component::Position>();
    view_loc.each([&registry, &dispatcher, dt](entt::entity e,
                                               component::LocationRequirement locationreqs,
                                               component::Movement& mov,
                                               component::Position pos) {
        if (close_enough(pos.position, locationreqs.desired_position, 5.f))
        {
            dispatcher.enqueue<event::FinishedRequirement>(event::FinishedRequirement{e, TAG_Location});
            registry.remove<component::LocationRequirement>(e);
        }
        else if (mov.desired_position.empty())
        {
            ai::find_path_astar(pos.position, locationreqs.desired_position, mov.desired_position);
        }

        // If we do not get closer to our target for an amount specified by max_time we fail the requirement
        auto distance = 0.f;
        for (int i = 1; i < mov.desired_position.size(); i++)
        {
            spdlog::get("agent")->warn("We are checking how close we are now");
            distance += glm::distance(mov.desired_position[i], mov.desired_position[i - 1]);
        }

        if (distance < locationreqs.closest_distance)
        {
            spdlog::get("agent")->warn("We are updating our closest distance");
            locationreqs.closest_distance = distance;
            locationreqs.elapsed_time     = 0.f;
        }
        else
        {
            spdlog::get("agent")->warn("We are adding time to our elapsed time");
            locationreqs.elapsed_time += dt;
            if (locationreqs.elapsed_time >= locationreqs.max_time)
            {
                spdlog::get("agent")->warn("We are failing our Locationrequirement");
                dispatcher.enqueue<event::RequirementFailure>(event::RequirementFailure{e, TAG_Location, ""});
                registry.remove<component::LocationRequirement>(e);
            }
        }
    });

    auto view_vis = registry.view<component::VisionRequirement, component::Vision>();
    view_vis.each([&registry, &dispatcher](entt::entity e, component::VisionRequirement visionreqs, component::Vision vision) {
        for (auto& entity : vision.seen)
        {
            if ((registry.get<component::Tags>(entity).tags & visionreqs.tags) == visionreqs.tags)
            {
                dispatcher.enqueue<event::FinishedRequirement>(event::FinishedRequirement{e, TAG_Vision});
                registry.remove<component::VisionRequirement>(e);
            }
        }

        // If we are not seeing the entity we are looking for
        if (registry.try_get<component::VisionRequirement>(e))
        {
            dispatcher.enqueue<event::RequirementFailure>(event::RequirementFailure{e, TAG_Vision, ""});
            registry.remove<component::VisionRequirement>(e);
        }
    });

    auto view_find =
        registry
            .view<component::FindRequirement, component::Strategy, component::Vision, component::Position, component::Movement>();
    view_find.each([&registry, dt, this](entt::entity e,
                                         component::FindRequirement& findreqs,
                                         component::Strategy& strategies,
                                         const component::Vision& vision,
                                         const component::Position& pos,
                                         component::Movement& mov) {
        findreqs.elapsed_time += dt;
        if (findreqs.elapsed_time >= findreqs.max_time)
        {
            m_context.dispatcher->enqueue<event::RequirementFailure>(event::RequirementFailure{e, TAG_Find, ""});
            registry.remove<component::FindRequirement>(e);
        }

        for (auto& entity : vision.seen)
        {
            if ((registry.get<component::Tags>(entity).tags & findreqs.tags) == findreqs.tags)
            {
                if (strategies.staged_strategies.size() != 0)
                {
                    registry.assign<component::LocationRequirement>(e,
                                                                    registry.get<component::Position>(entity).position,
                                                                    0.f,
                                                                    30.f,
                                                                    0.f);

                    strategies.staged_strategies.back().actions.back().target = entity;

                    m_context.dispatcher->enqueue<event::FinishedRequirement>(event::FinishedRequirement{e, TAG_Find});
                    registry.remove<component::FindRequirement>(e);
                    mov.desired_position.clear();
                }
                else
                {
                    m_context.dispatcher->enqueue<event::RequirementFailure>(
                        event::RequirementFailure{e, TAG_Find, "The entity's staged requirements list is empty"});
                }
                return;
            }
        }

        if (auto memories = registry.try_get<component::Memory>(e); memories)
        {
            for (auto& memory_container : memories->memory_container)
            {
                // Find a container matching our tag
                if ((memory_container.memory_tags & findreqs.tags == findreqs.tags) &&
                    memory_container.memory_tags & TAG_Location)
                {
                    auto& memory = memory_container.memory_storage.front();

                    if (auto* res = dynamic_cast<memory::ResourceLocation*>(memory.get()); res)
                    {
                        if (findreqs.desired_position != res->m_location && mov.desired_position.empty())
                        {
                            findreqs.desired_position = res->m_location;
                            ai::find_path_astar(pos.position, findreqs.desired_position, mov.desired_position);
                        }

                        if (close_enough(pos.position, findreqs.desired_position, 5.f))
                        {
                            res->m_number_of_entities = 0.f;
                            mov.desired_position.clear();
                        }
                        break;
                    }
                }
            }
        }

        if (close_enough(pos.position, findreqs.desired_position, 5.f))
        {
            registry.assign_or_replace<component::FindRequirement>(
                e,
                findreqs.tags,
                glm::vec3(m_context.rng->uniform(-m_context.scenario->bounds.x, m_context.scenario->bounds.x),
                          m_context.rng->uniform(-m_context.scenario->bounds.y, m_context.scenario->bounds.y),
                          0.f),30.f,0.f);
        }

        else if (mov.desired_position.empty())
        {
            // If this is the first run and we have no memories so our desired position is not set
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
        m_context.dispatcher->enqueue<event::FinishedRequirement>(event::FinishedRequirement{e, TAG_Tag});
    });
}
} // namespace cs::system
