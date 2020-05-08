#include "requirement.h"
#include "ai/path_finding.h"
#include "common_helpers.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/memories/resource_location.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace cs::system
{
void Requirement::initialize()
{
    m_context.dispatcher->sink<event::EntityDeleted>().connect<&Requirement::remove_requirements>(*this);
}

void Requirement::deinitialize()
{
    m_context.dispatcher->sink<event::EntityDeleted>().disconnect<&Requirement::remove_requirements>(*this);
}

void Requirement::update(float dt)
{
    CS_AUTOTIMER(Requirement System);
    /** TODO: Expose Locationrequirements to lua more */
    goto_location(dt);

    look_for_target(dt);

    find_target(dt);

    add_tag(dt);

    check_inventory(dt);
}

ISystem* Requirement::clone()
{
    return new Requirement(m_context);
}

void Requirement::remove_requirements(const event::EntityDeleted& event)
{
    m_context.registry->remove_if_exists<component::FindRequirement,
                                         component::LocationRequirement,
                                         component::TagRequirement,
                                         component::VisionRequirement,
                                         component::InventoryRequirement>(event.entity);
}

void Requirement::goto_location(float dt)
{
    auto view_loc = m_context.registry->view<component::LocationRequirement, component::Movement, component::Position>();
    view_loc.each([dt, this](const entt::entity e,
                             component::LocationRequirement& locationreqs,
                             component::Movement& mov,
                             const component::Position& pos) {
        if (close_enough(pos.position, locationreqs.desired_position, 5.f))
        {
            m_context.dispatcher->enqueue<event::FinishedRequirement>(event::FinishedRequirement{e, TAG_Location});
            m_context.registry->remove<component::LocationRequirement>(e);
        }
        else if (mov.desired_position.empty())
        {
            ai::find_path_astar(pos.position, locationreqs.desired_position, mov.desired_position, m_context.scenario->bounds);
        }

        /** If we do not get closer to our target for an amount specified by max_time we fail the requirement */
        auto distance = 0.f;
        for (int i = 1; i < mov.desired_position.size(); i++)
        {
            distance += glm::distance(mov.desired_position[i], mov.desired_position[i - 1]);
        }

        if (distance < locationreqs.closest_distance)
        {
            locationreqs.closest_distance = distance;
            locationreqs.elapsed_time     = 0.f;
        }
        else
        {
            locationreqs.elapsed_time += dt;
            if (locationreqs.elapsed_time >= locationreqs.max_time)
            {
                m_context.dispatcher->enqueue<event::RequirementFailure>(event::RequirementFailure{e, TAG_Location, ""});
                m_context.registry->remove_if_exists<component::LocationRequirement>(e);
            }
        }
    });
}

void Requirement::look_for_target(float dt)
{
    auto view_vis = m_context.registry->view<component::VisionRequirement, component::Vision>();
    view_vis.each([this](const entt::entity e, const component::VisionRequirement& visionreqs, const component::Vision& vision) {
        for (auto& entity : vision.seen)
        {
            auto tags = m_context.registry->try_get<component::Tags>(entity);
            if (tags && ((tags->tags & visionreqs.tags) == visionreqs.tags) && !(tags->tags & TAG_Delete))
            {
                m_context.dispatcher->enqueue<event::FinishedRequirement>(event::FinishedRequirement{e, TAG_Vision});
                m_context.registry->remove_if_exists<component::VisionRequirement>(e);
            }
        }

        /** If we are not seeing the entity we are looking for */
        if (m_context.registry->try_get<component::VisionRequirement>(e))
        {
            m_context.dispatcher->enqueue<event::RequirementFailure>(event::RequirementFailure{e, TAG_Vision, ""});
            m_context.registry->remove_if_exists<component::VisionRequirement>(e);
        }
    });
}

void Requirement::find_target(float dt)
{
    auto view_find = m_context.registry->view<component::FindRequirement,
                                              component::Strategy,
                                              component::Vision,
                                              component::Position,
                                              component::Movement>();

    view_find.each([dt, this](const entt::entity e,
                              component::FindRequirement& findreqs,
                              component::Strategy& strategies,
                              const component::Vision& vision,
                              const component::Position& pos,
                              component::Movement& mov) {
        findreqs.elapsed_time += dt;
        if (findreqs.elapsed_time >= findreqs.max_time)
        {
            m_context.dispatcher->enqueue<event::RequirementFailure>(event::RequirementFailure{e, TAG_Find, ""});
            m_context.registry->remove<component::FindRequirement>(e);
        }

        for (const auto entity : vision.seen)
        {
            if (!m_context.registry->valid(entity))
            {
                continue;
            }

            auto tags = m_context.registry->try_get<component::Tags>(entity);
            if (tags && ((tags->tags & findreqs.tags) == findreqs.tags) && !(tags->tags & TAG_Delete))
            {
                if (strategies.staged_strategies.size() != 0)
                {
                    m_context.registry->assign<component::LocationRequirement>(
                        e,
                        m_context.registry->get<component::Position>(entity).position,
                        0.f,
                        30.f,
                        0.f);

                    strategies.staged_strategies.back().actions.back().target = entity;

                    m_context.dispatcher->enqueue<event::FinishedRequirement>(event::FinishedRequirement{e, TAG_Find});
                    m_context.registry->remove_if_exists<component::FindRequirement>(e);
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

        if (auto memories = m_context.registry->try_get<component::Memory>(e); memories)
        {
            for (auto& memory_container : memories->memory_container)
            {
                /** Find a container matching our tag */
                if (((memory_container.memory_tags & findreqs.tags) == findreqs.tags) &&
                    memory_container.memory_tags & TAG_Location && !memory_container.memory_storage.empty())
                {
                    auto& memory = memory_container.memory_storage.front();

                    if (auto* res = dynamic_cast<memory::ResourceLocation*>(memory.get()); res)
                    {
                        if (findreqs.desired_position != res->m_location && mov.desired_position.empty())
                        {
                            findreqs.desired_position = res->m_location;
                            ai::find_path_astar(pos.position,
                                                findreqs.desired_position,
                                                mov.desired_position,
                                                m_context.scenario->bounds);
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
            m_context.registry->assign_or_replace<component::FindRequirement>(
                e,
                findreqs.tags,
                glm::vec3(m_context.rng->uniform(-m_context.scenario->bounds.x, m_context.scenario->bounds.x),
                          m_context.rng->uniform(-m_context.scenario->bounds.y, m_context.scenario->bounds.y),
                          0.f),
                30.f,
                0.f);
        }

        else if (mov.desired_position.empty())
        {
            /** If this is the first run and we have no memories so our desired position is not set */
            if (findreqs.desired_position == glm::vec3{0.f, 0.f, 0.f})
            {
                findreqs.desired_position =
                    glm::vec3(m_context.rng->uniform(-m_context.scenario->bounds.x, m_context.scenario->bounds.x),
                              m_context.rng->uniform(-m_context.scenario->bounds.y, m_context.scenario->bounds.y),
                              0.f);
            }

            ai::find_path_astar(pos.position, findreqs.desired_position, mov.desired_position, m_context.scenario->bounds);
        }
    });
}

void Requirement::add_tag(float dt)
{
    auto view_tag = m_context.registry->view<component::TagRequirement, component::Tags>();
    view_tag.each([this](const entt::entity e, const component::TagRequirement& tagreqs, component::Tags& tags) {
        tags.tags = ETag(tags.tags | tagreqs.tags);
        m_context.registry->remove<component::TagRequirement>(e);
        m_context.dispatcher->enqueue<event::FinishedRequirement>(event::FinishedRequirement{e, TAG_Tag});
    });
}

void Requirement::check_inventory(float dt)
{
    auto view_inventory = m_context.registry->view<component::InventoryRequirement, component::Inventory, component::Strategy>();
    view_inventory.each([this](const entt::entity e,
                               const component::InventoryRequirement& invreqs,
                               const component::Inventory& inv,
                               component::Strategy& strat) {
        if ((inv.tags & invreqs.tags) == invreqs.tags)
        {
            for (auto content : inv.contents)
            {
                if (auto tags = m_context.registry->try_get<component::Tags>(content);
                    tags && ((tags->tags & invreqs.tags) == invreqs.tags))
                {
                    strat.staged_strategies.back().actions.back().target = content;
                    m_context.registry->remove<component::InventoryRequirement>(e);
                    m_context.dispatcher->enqueue<event::FinishedRequirement>(event::FinishedRequirement{e, TAG_Inventory});
                    return;
                }
            }
        }

        m_context.dispatcher->enqueue<event::RequirementFailure>(event::RequirementFailure{e, TAG_Inventory});
        m_context.registry->remove<component::InventoryRequirement>(e);
    });
}
} // namespace cs::system
