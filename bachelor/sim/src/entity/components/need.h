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

    float vitality{};

    ETag tags{};
};

inline bool operator<(Need const& lhs, Need const& rhs)
{
    return ((100 / (lhs.status + 1)) * lhs.weight < (100 / (rhs.status + 1)) * rhs.weight);
}

inline bool operator>(Need const& lhs, Need const& rhs)
{
    return ((100 / (lhs.status + 1)) * lhs.weight > (100 / (rhs.status + 1)) * rhs.weight);
}

inline bool operator==(Need const& lhs, Need const& rhs)
{
    return lhs.name == rhs.name;
}

inline bool operator!=(Need const& lhs, Need const& rhs)
{
    return lhs.name != rhs.name;
}
} // namespace cs::ai
