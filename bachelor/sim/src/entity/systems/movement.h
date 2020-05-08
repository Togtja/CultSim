#pragma once
#include "system.h"

namespace cs::system
{
/**
 * Movement system is an necessary system for any movemnt in the simulation
 * It handels the updating of all the entities positions.
 * It also takes care of avoiding other object, that needs to be avoided
 */
class Movement : public ISystem
{
private:
    /** How many degrees we rotate when avoiding something */
    float avoid_rotation{35.f};

    /** The cool down for each avoidance check */
    float avoid_cd{0.14f};

    /** How far we avoid in the rotation */
    float avoid_dist{10.f};

    /** The distance we start avoiding a collition */
    float avoid_start{20.f};

public:
    using ISystem::ISystem;

    /**
     * Run every frame and updates the entity's position, and does collition if any
     *
     * @param dt The time step from the last frame
     */
    void update(float dt) override;

    /**
     * Draw the different debug utilities related to movement, such as pathfinding and vision
     */
    void update_imgui() override;

    ISystem* clone() override;
};
} // namespace cs::system
