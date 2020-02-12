#pragma once
#include <cstdint>

namespace cs::tags
{
enum ETag : uint64_t
{
    TAG_Food  = 1 << 1,
    TAG_Drink = 1 << 2,
    TAG_Sleep = 1 << 3,
    TAG_Joy   = 1 << 4,
};
} // namespace cs::tags
