#include "path_finding.h"

#include <spdlog/spdlog.h>

namespace cs::ai
{
glm::ivec2 world_to_grid(glm::vec2 pos, int grid)
{
    return {static_cast<int>(pos.x) / static_cast<int>(grid), static_cast<int>(pos.y) / static_cast<int>(grid)};
}

int path_heuristic(glm::ivec2 start, glm::ivec2 goal)
{
    auto p = start - goal;
    return p.x * p.x + p.y * p.y;
}

void reconstruct_path(const glm::ivec2& start,
                      const glm::ivec2& goal,
                      const glm::vec2& actual_goal,
                      std::vector<glm::vec3>& pos,
                      const robin_hood::unordered_flat_map<glm::ivec2, glm::ivec2>& a_star_grid)
{
    glm::ivec2 curr = goal;
    pos.emplace_back(glm::vec3(actual_goal, 0.f));
    do
    {
        pos.emplace_back(glm::vec3(curr * 32, 0));
        curr = a_star_grid.at(curr);
    } while (curr != start);
}

bool find_path_astar(glm::vec2 start_vec, glm::vec2 goal_vec, std::vector<glm::vec3>& poss, int accuracy)
{
    robin_hood::unordered_flat_map<glm::ivec2, glm::ivec2> a_star_grid{};
    robin_hood::unordered_flat_map<glm::ivec2, int> a_star_cost{};
    auto start_grid = world_to_grid(start_vec, accuracy);
    auto goal_grid  = world_to_grid(goal_vec, accuracy);
    glm::ivec2 start{start_grid.x, start_grid.y};
    glm::ivec2 goal{goal_grid.x, goal_grid.y};

    auto priority_func = [](const std::pair<int, glm::ivec2>& a, const std::pair<int, glm::ivec2>& b) {
        return a.first > b.first;
    };

    std::priority_queue<std::pair<int, glm::ivec2>, std::vector<std::pair<int, glm::ivec2>>, decltype(priority_func)> open(
        priority_func);

    open.emplace(0.f, start);
    a_star_grid[start] = start;
    a_star_cost[start] = 0;

    while (!open.empty())
    {
        glm::ivec2 curr = open.top().second;
        open.pop();
        if (curr == goal)
        {
            reconstruct_path(start, goal, goal_vec, poss, a_star_grid);
            return true;
        }
        glm::ivec2 next{};

        const auto max = glm::ivec2{curr.x + 1, curr.y + 1};
        const auto min = glm::ivec2{curr.x - 1, curr.y - 1};

        for (int x = min.x; x <= max.x; x++)
        {
            for (int y = min.y; y <= max.y; y++)
            {
                next.x = x;
                next.y = y;
                if (next == curr)
                {
                    continue;
                }
                int new_cost = a_star_cost[curr] + 1; // + cost of graph node needs to be computed
                if (a_star_cost.find(next) == a_star_cost.end() || new_cost < a_star_cost[next])
                {
                    // spdlog::warn("Found a node to check");
                    a_star_cost[next] = new_cost;
                    a_star_grid[next] = curr;
                    int priority      = new_cost + path_heuristic(next, goal);

                    open.emplace(priority, next);
                }
            }
        }
    }
    return false;
}
} // namespace cs::ai
