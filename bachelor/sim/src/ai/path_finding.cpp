#include "path_finding.h"
glm::ivec2 world_to_grid(glm::vec2 pos, int grid)
{
    return {static_cast<int>(pos.x) / static_cast<int>(grid), static_cast<int>(pos.y) / static_cast<int>(grid)};
}

float path_heuristic(PathGrid pos, PathGrid pos2)
{
    float x = pos.x - pos2.x;
    float y = pos.y - pos2.y;
    return x * x + y * y;
}
