#pragma once

#include "system.h"

namespace cs::system
{
/**
 * Updates status of needs and moves needs into a list of pressing needs
 *
 * Runs every frame and checks every entity with a Needs component
 * Every needs status is lowered by an amount defined in the need multiplied with deltatime
 * If the need drops under 50%, it is then added to a list of pressing needs
 *
 * This System will send out events if the followign occurs:
 * The System adds a need to the list of vital needs                                             : event::NeedBecameCritical
 * The System adds a need to the list of vital needs when there were previously no vital needs   : event::SwitchNeedContext
 * The System removes a need from the list of vital needs                                        : event::NeedNoLongerCritical
 * The System removes a need from the list of vital needs and no more vital needs remain         : event::SwitchNeedContext
 */
class Need : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
