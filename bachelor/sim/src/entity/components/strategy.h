#pragma once

#include "entity/actions/action.h"
#include "tags.h"

#include <string>
#include <vector>

namespace cs::ai
{
/** TODO: Add Documentation */
/**
 * A Strategy is a container used to store actions which is used by the action system to run through said actions
 *
 * Strategies contain a series of actions which are run in sequential order, each action adding it's requirements to the
 * strategies. These are then resolved before the action is run. Strategies are stored in the strategy component, and only one of
 * them runs at a time.
 */
struct Strategy
{
    std::string name{};

    /** How well the strategy will solve a given need. This is used by the mitigation system and should not be altered by the user
     * directly. */
    float desirability{};

    /** The index of the current action that is being worked on, counting from the back of the vector */
    int working_on_action{0};

    /** The tags of the strategy, which are used to determine it's desireability by how closely they match those of the given need
     */
    ETag tags{};

    /** The tags of potential entities that the Strategy is looking for */
    ETag target_tags{};

    /** Tags an entity needs to have in order to be able to run this strategy */
    ETag prerequisites{};

    /** Requirements that the strategy needs to fullfill before running the current action. These are set by the action system */
    ETag requirements{};

    std::vector<action::Action> actions{};
};

inline bool operator==(Strategy const& lhs, Strategy const& rhs)
{
    return lhs.name == rhs.name;
}

inline bool operator<(Strategy const& lhs, Strategy const& rhs)
{
    return lhs.desirability < rhs.desirability;
}

inline bool operator>(Strategy const& lhs, Strategy const& rhs)
{
    return lhs.desirability > rhs.desirability;
}
} // namespace cs::ai
