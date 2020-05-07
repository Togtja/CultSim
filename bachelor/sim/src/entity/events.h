#pragma once
#include "entity/components/tags.h"
#include "entity/memories/memory.h"
#include <memory>

#include <entt/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs::event
{
/** TODO: Add some brief documentation */

/**
 * Event for when entities have arrived at their move to destination
 */
struct ArrivedAtDestination
{
    /** Entity that arrived at a destination */
    entt::entity entity{};
    glm::vec3 position{};
};

/**
 * Event for when a scenario has finished loading
 */
struct ScenarioLoaded
{
};

struct SensedEntity
{
    entt::entity entity{};
};

struct NeedBecameCritical
{
    entt::entity entity{};

    cs::ETag tag{};

    float status{};

    float weight{};
};

struct NeedNoLongerCritical
{
    entt::entity entity{};

    cs::ETag tag{};

    float status{};

    float weight{};
};

struct SwitchNeedContext
{
    entt::entity entity{};
};

struct FinishedRequirement
{
    entt::entity entity{};

    cs::ETag requirement{};
};

struct RequirementFailure
{
    entt::entity entity{};

    cs::ETag requirement{};

    std::string error{};
};

struct CreatedMemory
{
    entt::entity entity{};

    std::shared_ptr<memory::IMemory> memory{};
};

struct EntitySpawned
{
    std::string_view entity_type{};
    entt::entity entity{};
    glm::vec2 position{};
};

struct EntityDeleted
{
    entt::entity entity{};
};

struct EntityPickedUp
{
    entt::entity owner{};

    entt::entity target{};

    ETag tags{};
};

struct EntityBorn
{
    entt::entity new_born{};
};
} // namespace cs::event
