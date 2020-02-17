#include "path_finding.h"
static robin_hood::unordered_flat_map<PathGrid, PathGrid> a_star_grid{};
static robin_hood::unordered_flat_map<PathGrid, float> a_star_cost{};

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
void reconstruct_path(PathGrid start, PathGrid goal, std::vector<glm::vec3>& pos)
{
    PathGrid curr = goal;
    do
    {
        pos.emplace_back(glm::vec3(curr.x * 32, curr.y * 32, 0));
        curr = a_star_grid[curr];
    } while (curr != start);
}
