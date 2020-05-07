#pragma once
#include "robin_hood.h"
#include "system.h"

#include <random>

#include <glm/vec2.hpp>

namespace cs::system
{
/**
 * The sensor system updates the components related to sensing and populate them with what they have sensed
 *
 * Currently there are only Vision Components with 360 degrees of vision. This can be expaneded upon, and should be.
 */
class Sensor : public ISystem
{
private:
    /** Number of threads per parallel execution task when computing vision */
    static inline constexpr int s_chunk_size{128};

    /** Uniform grid for collision checks */
    robin_hood::unordered_flat_map<int, std::vector<entt::entity>> m_collision_grid{};

public:
    using ISystem::ISystem;

    /**
     * Check if two positions are visible based on a radius
     *
     * @param pos The first position
     * @param pos2 The second position
     * @param rad How far from the first position can we see, the radius
     * @return True if pos2 is visible from pos
     */
    bool is_visible(const glm::vec2& pos, const glm::vec2& pos2, float rad);

    /**
     * Check if two positions are visible given a bounded map
     *
     * @param pos The first position
     * @param pos2 The second position
     * @param rad How far from the first position can we see, the radius
     * @param bounds The bounds of the level
     * @return True if pos2 is visible from pos
     */
    bool is_visible_bound(const glm::vec2& pos, const glm::vec2& pos2, float rad, const glm::vec2& bounds);

    /**
     * Check if two circles are colliding
     *
     * @param pos The first circle center position
     * @param pos2 The second circle center position
     * @param size Radius of the first circle
     * @param size2 Radius of the second circle
     * @return True if the circles overlap
     */
    bool is_colliding(const glm::vec2& pos, const glm::vec2& pos2, float size, float size2);

    /**
     * Check if two circles are colliding across bounds
     *
     * @param pos The first circle center position
     * @param pos2 The second circle center position
     * @param size Radius of the first circle
     * @param size2 Radius of the second circle
     * @param bounds The bounds of the level
     * @return True if the circles overlap
     */
    bool is_colliding_bound(const glm::vec2& pos, const glm::vec2& pos2, float size, float size2, const glm::vec2& bounds);

    void update(float dt) override;

    void update_imgui() override;

    ISystem* clone() override;
};

} // namespace cs::system
