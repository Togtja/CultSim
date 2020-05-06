#pragma once
#include "entity/components/action_sequence.h"
#include "entity/components/goal.h"
#include "system.h"

#include <vector>

namespace cs::system
{
/**
 * The Goal Oriented Behaviour System takes a list of goals and uses predefined functions to choose the most favorable action.
 *
 * The System works on the Goals component and the Actions component. It goes through the list of Actionsequences and sees how
 * they will impact our goals. The ActionSequence with the most favourable outcome across all goals will then be chosen as the
 * next action sequence to complete, and be placed into current_action_sequence in the Actions component.
 *
 * @note The systems algorythm makes number of agents * (number of actions * number of goals) runs, with each run involving many
 * different functions. Despite this, the system is still rather quick when compared to other systems.
 */
class GOB : public ISystem
{
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;

private:
    /**
     * Calculates how dissatisfactory the world state is affter we run a sequence of actions
     *
     * @param e The entity who is running the action
     * @param action The Sequence of actions to be taken
     * @param goals A collection of all goals that the entity has
     *
     * @return The total negative impact from the weight of our goals after we run the action
     */
    float calculate_discontentment(entt::entity e, const gob::ActionSequence& action, const std::vector<gob::Goal>& goals);
};
} // namespace cs::system
