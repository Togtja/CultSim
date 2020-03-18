#pragma once
#include "robin_hood.h"
#include "system.h"

#include <random>

#include <glm/vec2.hpp>

namespace cs::system
{
class Sensor : public ISystem
{
private:
    robin_hood::unordered_flat_map<int, std::vector<entt::entity>> m_collision_grid{};

public:
    using ISystem::ISystem;

    bool is_visible(const glm::vec2& pos, const glm::vec2& pos2, float rad);
    bool is_visible_bound(const glm::vec2& pos, const glm::vec2& pos2, float rad, const glm::ivec2& bounds);

    bool is_colliding(const glm::vec2& pos, const glm::vec2& pos2, float size, float size2);
    bool is_colliding_bound(const glm::vec2& pos, const glm::vec2& pos2, float size, float size2, const glm::ivec2& bounds);

    void update(float dt) override;

private:
    bool close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold);
};

} // namespace cs::system
