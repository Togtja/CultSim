#pragma once
#include "tags.h"

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

    tags::ETag tags{};

    bool operator<(Need const& need2)
    {
        return ((100 / (status + 0.01)) * weight < (100 / (need2.status + 0.01)) * need2.weight);
    }

    bool operator>(Need const& need2)
    {
        return ((100 / status + 0.01)) * weight > (100 / (need2.status+0.01))*need2.weight);
    }

    bool operator==(Need const& need2)
    {
        return name == need2.name;
    }
};

} // namespace cs::ai
