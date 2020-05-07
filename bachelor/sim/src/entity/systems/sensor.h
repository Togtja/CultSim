#pragma once
#include "robin_hood.h"
#include "system.h"

#include <random>

#include <glm/vec2.hpp>

namespace cs::system
{
/** TODO: Documentation */
class Sensor : public ISystem
{
private:
    static inline constexpr int s_chunk_size{128};

    robin_hood::unordered_flat_map<int, std::vector<entt::entity>> m_collision_grid{};

public:
    using ISystem::ISystem;

    bool is_visible(const glm::vec2& pos, const glm::vec2& pos2, float rad);

    bool is_visible_bound(const glm::vec2& pos, const glm::vec2& pos2, float rad, const glm::vec2& bounds);

    bool is_colliding(const glm::vec2& pos, const glm::vec2& pos2, float size, float size2);

    bool is_colliding_bound(const glm::vec2& pos, const glm::vec2& pos2, float size, float size2, const glm::vec2& bounds);

    void update(float dt) override;

    void update_imgui() override;

    ISystem* clone() override;

private:
    bool close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold);
};

} // namespace cs::system
