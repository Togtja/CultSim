#pragma once

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
} // namespace cs::event
