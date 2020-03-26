#pragma once

#include "system.h"

namespace cs::system
{
/**
 * System that tracks vital needs and begins ticking down the owners HP should they fall to 0
 *
 * The Health system keeps track of the owners stats, looking for stats that are defined as vital in it's corresponding component.
 * Should any such need's status drop to 0, the system begins ticking down the components health stat by an amount defined in the
 * component. Once an entities health reaches 0, it is then deleted from the simulation
 *
 * @note Health ticks down once for each vital stat that has fallen to 0, so the more needs are currently unfullfilled, the faster
 * our health drops
 */
class Health : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
