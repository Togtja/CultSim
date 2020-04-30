#pragma once

#include <glm/glm.hpp>

bool is_visible_bound_dist(const glm::vec2& pos, const glm::vec2& pos2, const glm::vec2& bounds, float rad);

bool is_visible_bound_sq(const glm::vec2& pos, const glm::vec2& pos2, const glm::ivec2& bounds, float rad);

bool is_visible_bounded(const glm::vec2& pos, const glm::vec2& pos2, const glm::vec2& bounds, float rad);

constexpr int func_0_vars()
{
    return std::pow(4, 2) / 8;
}

constexpr int func_1_vars(int pow)
{
    return std::pow(4, pow) / 8;
}

constexpr int func_2_vars(int pow, int divisor)
{
    return std::pow(4, pow) / divisor;
}
