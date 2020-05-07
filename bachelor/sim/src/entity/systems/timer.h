#pragma once

#include "system.h"

namespace cs::system
{
/**
 * System which operates on a timer component to allow the running of a timed action on an entity one or more times
 *
 * The Timer system allows us to associate a timed action with an entity via a timer component, which will be removed once the
 * action has been completed a certain number of times. The Timer can take any action on the entity that only requires the
 * registry and entity itself to be properly run. It also has functionality to keep running indefinitely
 */
class Timer : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
