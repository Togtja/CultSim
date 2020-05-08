#pragma once
#include "system.h"

namespace cs::system
{
/**
 * The Goal system will run through all goals an entity has, updating them and sorting them by weight
 *
 * The system uses the Goal component to gain access to all an entities goals, which have their age updated via the use of dt, and
 * are then sorted based on their weightfunction, with the one with the highest weight being placed in the back of the vector
 */
class Goal : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
