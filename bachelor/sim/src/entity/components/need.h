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

    std::vector<std::string> tag{};
};

} // namespace cs::ai
