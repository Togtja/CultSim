#pragma once

#include <glm/glm.hpp>

bool is_visible_bound_dist(const glm::vec2& pos, const glm::vec2& pos2, const glm::vec2& bounds, float rad);

bool is_visible_bound_sq(const glm::vec2& pos, const glm::vec2& pos2, const glm::ivec2& bounds, float rad);

bool is_visible_bounded(const glm::vec2& pos, const glm::vec2& pos2, const glm::vec2& bounds, float rad);

template<typename T>
constexpr T ipow(T num, unsigned int pow)
{
    return (pow >= sizeof(unsigned int) * 8) ? 0 : pow == 0 ? 1 : num * ipow(num, pow - 1);
}

constexpr int func_0_vars()
{
    return ipow(4, 2) / 8;
}

constexpr int func_1_vars(int pow)
{
    return ipow(4, pow) / 8;
}

constexpr int func_2_vars(int pow, int divisor)
{
    return ipow(4, pow) / divisor;
}
