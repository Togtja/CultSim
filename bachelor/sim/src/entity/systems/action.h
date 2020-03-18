#pragma once
#include "entity/events.h"
#include "random_engine.h"
#include "system.h"

#include <entt/signal/dispatcher.hpp>

namespace cs::system
{
/**
 * Action system goes through each entity and runs the actions in the first strategy of the staged strategy list
 *
 * The Action system goes through each entity, and finds the first strategy in the list of staged strategies.
 * It then goes to the first action, and if it has a requirement, it adds a corresponding component to the entity
 * The action system then ignores entities with requirement components
 * Once the requirement has been taken care off, the action system begins ticking down the time left on the action
 * When the action is done, the system then runs a check to see if the action succeeds, and then removes it from the list of
 * actions Once the strategy is out of actions, if the need has not yet been fullfilled, the next action is chosen
 */
class Action : public ISystem
{
public:
    Action(SystemContext context) : ISystem(context)
    {
        context.dispatcher->sink<event::RequirementFailure>().connect<&Action::abort_strategy>(this);
    }

    ~Action()
    {
        m_context.dispatcher->sink<event::RequirementFailure>().disconnect<&Action::abort_strategy>(this);
    }
    
    void update(float dt) override;

    void abort_strategy(const event::RequirementFailure& event);
};
} // namespace cs::system
