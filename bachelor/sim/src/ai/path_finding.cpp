#include "path_finding.h"

#include <spdlog/spdlog.h>

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

bool path_finding2(glm::vec2 start_vec, glm::vec2 goal_vec, std::vector<glm::vec3>& poss)
{
    auto pos       = world_to_grid(start_vec, 32);
    auto goal_grid = world_to_grid(goal_vec, 32);
    PathGrid start{pos.x, pos.y};
    PathGrid goal{goal_grid.x, goal_grid.y};

    using pair = std::pair<float, PathGrid>;

    auto priority_func = [](const pair& a, const pair& b) {
        return a.first > b.first;
    };
    std::priority_queue<pair, std::vector<pair>, decltype(priority_func)> open(priority_func);

    open.emplace(0.f, start);
    a_star_grid[start] = start;
    a_star_cost[start] = 0;

    while (!open.empty())
    {
        PathGrid curr = open.top().second;
        open.pop();
        if (curr == goal)
        {
            reconstruct_path(start, goal, poss);
            return true;
        }
        PathGrid next{};
        float curr_low = ~0ull;
        // TODO: Improve by first checking in the heuristic direction
        auto max = world_to_grid(curr + 32);
        auto min = world_to_grid(curr - 32);
        for (int x = min.x; x < max.x; x++)
        {
            for (int y = min.y; y < max.y; y++)
            {
                next.x         = x;
                next.y         = y;
                float new_cost = a_star_cost[curr] + 1; // + cost of graph node needs to be computed
                if (a_star_cost.find(next) == a_star_cost.end() || new_cost < a_star_cost[next])
                {
                    spdlog::warn("Found a node to check");
                    a_star_cost[next] = new_cost;
                    a_star_grid[next] = curr;
                    float priority    = new_cost + path_heuristic(next, goal);

                    open.emplace(priority, next);
                }
            }
        }
    }
    return false;
}