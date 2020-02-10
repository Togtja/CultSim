#pragma once

#include <string>
#include <vector>

namespace cs::ai
{

struct Need
{
    std::string name{};

    float weight{};

    float status{};

    float decay_rate{};

    std::vector<std::string> tag{};
};

} // namespace cs::ai
