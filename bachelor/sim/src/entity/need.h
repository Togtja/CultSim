#pragma once

#include <string>

namespace cs::ai
{

struct Need
{
    std::string name{};

    float weight{};

    float status{};

    vector<std::string> tag{};
}

} // namespace cs::ai
