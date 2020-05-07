#pragma once
#include "entity/events.h"
#include "system.h"

namespace cs::system
{
/**
 * Action system goes through each entity and runs the actions in the first strategy of the staged strategy list
 *
 * The Action system goes through each entity, and finds the first strategy in the list of staged strategies.
 * It then goes to the first action, and if it has a requirement, it adds a corresponding component to the entity.
 * The action system then ignores entities with requirement components.
 * Once the requirement has been taken care off, the action system begins ticking down the time left on the action.
 * When the action is done, the system then runs a check to see if the action succeeds, and then removes it from the list of
 * actions. Once the strategy is out of actions, if the need has not yet been fullfilled, the next action is chosen.
 */
class Action : public ISystem
{
public:
    using ISystem::ISystem;

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

private:
    /**
     * abort_strategy runs abort() on the strategy being currently worked on
     *
     * The function is used by the system upon recieving an event from the Requirement system that one of the actions requirements
     * could not be completed. This causes the system to abort the strategy, and reduce it's desireability by one before resorting
     * the list of staged strategies. This helps in ensuring that if a there are more than one staged strategy, a strategy that
     * might have initially been a worse choice can now be attempted, seeing as the most optimal strategy has failed. If there is
     * only one strategy, this will instead simply restart it from the beginning.
     *
     * @param event The event send by the requirementsystem about the failure in completing the requirement
     */
    void abort_strategy(const event::RequirementFailure& event);

    /**
     * delete_target goes through all actions and calls abort() on those that have the target specified in the event
     *
     * The Function will go through every single action in every single strategy in staged strategies and call abort on them if
     * they have a target set to that specified in the event. In practice, most actions will not have a target until they are
     * running, so this will only run it's full course a few times.
     * The actions target is then reset to be null, and the strategy is reset.
     *
     * @param event The event send by the Deletionsystem about the entity which has been deleted
     */
    void delete_target(const event::EntityDeleted& event);

    /**
     * picked_up_entity goes through every strategy and calls abort() on those that target the entity specified in the event
     *
     * This function has much the same effect as delete_target() finding the current strategy and current action, and calling
     * abort and resetting it should the target be equal to the entity that has been picked up by another actor.
     * This is to avoid issues where actors targeting the same entity will attempt to eat the entity out of eachothers backpacks,
     * or similar errors.
     *
     * @param event The event send by an action that has just picked up said an entity
     */
    void picked_up_entity(const event::EntityPickedUp& event);
};

} // namespace cs::system
