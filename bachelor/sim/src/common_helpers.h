#pragma once

#include <cstdint>

namespace cs
{
int count_set_bits(uint64_t x)
{
    int sum{};
    while (x != 0)
    {
        sum += x & 1;
        x >>= 1;
    }
    return sum;
}
} // namespace cs
