#pragma once
#include "entity/components/tags.h"

#include <entt/fwd.hpp>
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
    glm::vec3 position{};
};

struct SensedEntity
{
    entt::entity entity{};
};

struct NeedCritical
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
} // namespace cs::event
