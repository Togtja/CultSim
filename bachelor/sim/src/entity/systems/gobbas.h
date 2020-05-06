#pragma once
#include "system.h"

namespace cs::system
{
/** TODO: Documentation */
/**
 * The Goal Oriented Behaviour Based Action System runs a sequence of actions
 *
 * The GOBBAS utilizes the action component to run the actionsequence placed in the current_action_sequence.
 * It runs through every action in that sequence, and has built in error handling, rerunning parts of actions,
 * entire actions, or even the entire sequence if necessary. If that does not solve the issues, it will remove the
 * action sequence from the current_action_sequence, and inform the user that an error has occured, alongside the error that
 * occured.
 *
 * @note this system runs a lot of custom functions every frame. This
 */
class GOBBAS : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
