#pragma once

#include "system.h"

#include <string>
#include <utility>

#include <entt/entity/registry.hpp>
#include <glm/vec3.hpp>

namespace cs::system
{
/** TODO: Documentation */
class Reproduction : public ISystem
{
    struct Child
    {
        std::string type{};
        glm::vec3 position{};
        bool is_egg{false};

        std::string parent_type{};
        std::pair<entt::entity, entt::entity> parents{entt::null, entt::null};
    };

public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
