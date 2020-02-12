#pragma once

#include <string>
#include <vector>

namespace cs::ai
{
struct Strategy
{
    std::string name;

    float desirability{};

    std::vector<std::string> tags{};

    bool operator==(Strategy const& strategy2)
    {
        return name == strategy2.name
    }

    // TODO: Actual functionality
};
} // namespace cs::ai
