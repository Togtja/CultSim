#pragma once
#include <cstdint>

namespace cs::tags
{
enum ETag : uint64_t
{
    TAG_Food     = 1 << 1,
    TAG_Drink    = 1 << 2,
    TAG_Sleep    = 1 << 3,
    TAG_Joy      = 1 << 4,
    TAG_Location = 1 << 5,
    TAG_Find     = 1 << 6,
    TAG_Vision   = 1 << 7,

    TAG_Avoidable = 1 << 8
};
} // namespace cs::tags
