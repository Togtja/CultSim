#include "functions.h"

bool is_visible_bound_dist(const glm::vec2& pos, const glm::vec2& pos2, const glm::vec2& bounds, float rad)
{
    if (glm::distance(pos, pos2) <= rad)
    {
        return true;
    }
    if (pos.x - rad < -bounds.x)
    {
        auto b_pos = pos + glm::vec2(2.f * bounds.x, 0);
        if (glm::distance(b_pos, pos2) <= rad)
        {
            return true;
        }
    }
    if (pos.x + rad > bounds.x)
    {
        auto b_pos = pos - glm::vec2(2.f * bounds.x, 0);
        if (glm::distance(b_pos, pos2) <= rad)
        {
            return true;
        }
    }
    if (pos.y - rad < -bounds.y)
    {
        auto b_pos = pos + glm::vec2(0, 2.f * bounds.y);
        if (glm::distance(b_pos, pos2) <= rad)
        {
            return true;
        }
    }
    if (pos.y + rad > bounds.y)
    {
        auto b_pos = pos - glm::vec2(0, 2.f * bounds.y);
        if (glm::distance(b_pos, pos2) <= rad)
        {
            return true;
        }
    }
    return false;
}

bool is_visible_bound_sq(const glm::vec2& pos, const glm::vec2& pos2, const glm::ivec2& bounds, float rad)
{
    float x    = pos.x - pos2.x;
    float y    = pos.y - pos2.y;
    float rad2 = rad * rad;
    if (x * x + y * y <= rad2)
    {
        return true;
    }
    if (pos.x - rad < -bounds.x)
    {
        auto b_pos = pos + glm::vec2(2.f * bounds.x, 0);
        x          = b_pos.x - pos2.x;
        y          = b_pos.y - pos2.y;
        if (x * x + y * y <= rad2)
        {
            return true;
        }
    }
    if (pos.x + rad > bounds.x)
    {
        auto b_pos = pos - glm::vec2(2.f * bounds.x, 0);
        x          = b_pos.x - pos2.x;
        y          = b_pos.y - pos2.y;
        if (x * x + y * y <= rad2)
        {
            return true;
        }
    }
    if (pos.y - rad < -bounds.y)
    {
        auto b_pos = pos + glm::vec2(0, 2.f * bounds.y);
        x          = b_pos.x - pos2.x;
        y          = b_pos.y - pos2.y;
        if (x * x + y * y <= rad2)
        {
            return true;
        }
    }
    if (pos.y + rad > bounds.y)
    {
        auto b_pos = pos - glm::vec2(0, 2.f * bounds.y);
        x          = b_pos.x - pos2.x;
        y          = b_pos.y - pos2.y;
        if (x * x + y * y <= rad2)
        {
            return true;
        }
    }
    return false;
}

bool is_visible_bounded(const glm::vec2& pos, const glm::vec2& pos2, const glm::vec2& bounds, float rad)
{
    glm::vec2 d = glm::abs(pos2 - pos) / bounds * 2.f;

    if (d.x > 0.5f)
    {
        d.x = 1.f - d.x;
    }

    if (d.y > 0.5f)
    {
        d.y = 1.f - d.y;
    }

    d *= bounds * 2.f;
    return (d.x * d.x + d.y * d.y) <= rad * rad;
}
