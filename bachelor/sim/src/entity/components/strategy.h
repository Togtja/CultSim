#pragma once

#include "entity/actions/action.h"
#include "tags.h"

#include <string>
#include <vector>

namespace cs::ai
{
struct Strategy
{
    std::string name;

    float desirability{};

    ETag prerequisites{};

    ETag tags{};

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
