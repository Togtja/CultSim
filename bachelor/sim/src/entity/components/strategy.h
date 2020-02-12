#pragma once

#include "tags.h"

#include <string>
#include <vector>

namespace cs::ai
{
struct Strategy
{
    std::string name;

    float desirability{};

    tags::ETag requirements{};

    tags::ETag tags{};

    bool operator==(Strategy const& strategy2)
    {
        return name == strategy2.name;
    }

    bool operator<(Strategy const& strategy2)
    {
        return desirability < strategy2.desirability;
    }
    // TODO: Actual functionality
};
} // namespace cs::ai
