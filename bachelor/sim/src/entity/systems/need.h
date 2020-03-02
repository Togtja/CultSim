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
*/
class Need : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;
};
} // namespace cs::system
