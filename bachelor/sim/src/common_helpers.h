#pragma once

#include <cstdint>
#include "glm/gtc/epsilon.hpp"

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

bool close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold)
{
    glm::bvec2 boolvec = glm::epsilonEqual(pos, pos2, glm::vec2(threshold));
    return boolvec.x && boolvec.y;
}
} // namespace cs
