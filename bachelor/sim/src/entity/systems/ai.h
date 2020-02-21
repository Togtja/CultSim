#pragma once
#include "robin_hood.h"
#include "system.h"

#include <random>

#include <glm/vec2.hpp>

namespace cs::system
{
class AI : public ISystem
{
public:
    using ISystem::ISystem;

    bool is_visible(glm::vec2 pos, glm::vec2 pos2, float rad);

    bool is_colliding(glm::vec2 pos, glm::vec2 pos2, float size, float size2);

    void update(float dt) override;

private:
    bool close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold);
    robin_hood::unordered_flat_map<int, std::vector<entt::entity>> collision_grid{};
};

} // namespace cs::system
