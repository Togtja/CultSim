#pragma once

#include "entity/actions/action.h"
#include "tags.h"

#include <string>
#include <vector>

namespace cs::ai
{
/** TODO: Add Documentation */
struct Strategy
{
    std::string name{};

    float desirability{};

    int working_on_action{0};

    ETag tags{};

    ETag target_tags{};

    ETag prerequisites{};

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
