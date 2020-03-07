#pragma once

#include "memory.h"

#include <glm/vec3.hpp>
#include <entt/entity/registry.hpp>

namespace cs
{
class EntityMemory : public IMemory
{
    EntityMemory(ETag tags, float time_of_creation, glm::vec3 location, entt::entity target) :
        IMemory(tags, time_of_creation),
        m_location(location),
        m_target(target){};

    glm::vec3 m_location{};
    entt::entity m_target = entt::null;

    void update(float dt) override;
};
} // namespace cs