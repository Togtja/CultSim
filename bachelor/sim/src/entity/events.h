#pragma once
#include "entity/components/tags.h"
#include "entity/memories/memory.h"
#include <memory>

#include <entt/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs::event
{
/**
 * Event for when entities have arrived at their move to destination
 */
struct ArrivedAtDestination
{
    /** Entity that arrived at a destination */
    entt::entity entity{};

    /** Position they arrived at */
    glm::vec3 position{};
};

/**
 * Event for when a scenario has finished loading
 */
struct ScenarioLoaded
{
};

/**
 * Event for when an entity is sensed
 */
struct SensedEntity
{
    /** Entity that was sensed */
    entt::entity entity{};
};

/**
 * Event for when an entity's need has become critical
 */
struct NeedBecameCritical
{
    /** The entity whose need changed */
    entt::entity entity{};

    /** Tag of the need */
    cs::ETag tag{};

    /** Status of the need */
    float status{};

    /** Weight of the need */
    float weight{};
};

/**
 * Event when a critical entity need is no longer critical
 */
struct NeedNoLongerCritical
{
    /** The entity whose need changed */
    entt::entity entity{};

    /** Tag of the need */
    cs::ETag tag{};

    /** Status of the need */
    float status{};

    /** Weight of the need */
    float weight{};
};

/**
 * Event when the need system switches from doing pressing to leisure needs on an entity
 */
struct SwitchNeedContext
{
    /** The entity switching context */
    entt::entity entity{};
};

/**
 * Event when an entity action or strategy has finished a requirement
 */
struct FinishedRequirement
{
    /** Entity executing requirement */
    entt::entity entity{};

    /** The requirement tags */
    cs::ETag requirement{};
};

/**
 * Event when an entity action or strategy has failed a requirement
 */
struct RequirementFailure
{
    /** Entity executing requirement */
    entt::entity entity{};

    /** The requirement tags */
    cs::ETag requirement{};

    /** The error message */
    std::string error{};
};

/**
 * Event when a new memory is created for an entity
 */
struct CreatedMemory
{
    /** The entity that got a memory */
    entt::entity entity{};

    std::shared_ptr<memory::IMemory> memory{};
};

/**
 * Event when a new entity is spawned
 */
struct EntitySpawned
{
    /** The type of entity, I.E. a deer */
    std::string_view entity_type{};

    /** The entity id of the newly spawned entity */
    entt::entity entity{};

    /** The current position of the entity (if any) */
    glm::vec2 position{};
};

/**
 * Event when an entity is deleted
 */
struct EntityDeleted
{
    /** The deleted entity handle, it's valid during the event processing, but not later */
    entt::entity entity{};
};

/**
 * Event when an entity is picked up
 */
struct EntityPickedUp
{
    /** The owner who picked it up */
    entt::entity owner{};

    /** The picked up entity */
    entt::entity target{};

    ETag tags{};
};

/**
 * Event when an entity is born from a pregnant parent or egg
 */
struct EntityBorn
{
    /** The born entity */
    entt::entity new_born{};
};
} // namespace cs::event
