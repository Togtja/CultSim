#include "path_finding.h"

#include <spdlog/spdlog.h>

namespace cs::ai
{
glm::ivec2 world_to_grid(const glm::vec2& pos, int grid)
{
    return {static_cast<int>(pos.x) / grid, static_cast<int>(pos.y) / grid};
}

int path_heuristic(glm::ivec2 start, glm::ivec2 goal)
{
    auto p = start - goal;
    return p.x * p.x + p.y * p.y;
}

void reconstruct_path(const glm::ivec2& start,
                      const glm::ivec2& grid_goal,
                      const glm::vec2& goal,
                      std::vector<glm::vec3>& pos,
                      const int accuracy,
                      const robin_hood::unordered_flat_map<glm::ivec2, glm::ivec2>& a_star_grid)
{
    pos.emplace_back(glm::vec3(goal, 0.f));
    glm::ivec2 curr = grid_goal;
    do
    {
        pos.emplace_back(glm::vec3(curr * accuracy, 0));
        curr = a_star_grid.at(curr);
    } while (curr != start);
}

bool find_path_astar(const glm::vec2& start_vec,
                     const glm::vec2& goal_vec,
                     std::vector<glm::vec3>& poss,
                     const glm::vec2& bonds,
                     const int accuracy)
{
    robin_hood::unordered_flat_map<glm::ivec2, glm::ivec2> a_star_grid{};
    robin_hood::unordered_flat_map<glm::ivec2, int> a_star_cost{};
    auto start_grid = world_to_grid(start_vec, accuracy);
    auto goal_grid  = world_to_grid(goal_vec, accuracy);

    auto bonds_grid = world_to_grid(bonds, accuracy);
    glm::ivec2 alt_goal{goal_grid};
    // Cheking for horizontal
    if (goal_grid.x < 0)
    {
        // If bonds min/max is (-3,-3)(3,3) and goal is (-2, y), then alt goal is (3*2+1 + -2) = 5
        alt_goal.x = (bonds_grid.x * 2 + 1) + goal_grid.x;
    }
    else if (goal_grid.x > 0)
    {
        // If bonds min/max is (-3,-3)(3,3) and goal is (2, y), then alt goal is (-(3*2+1) + 2) = -5
        alt_goal.x = -(bonds_grid.x * 2 + 1) + goal_grid.x;
    }
    if (goal_grid.y < 0)
    {
        // If bonds min/max is (-3,-3)(3,3) and goal is (x, -3), then alt goal is (3*2+1 + -3) = 4
        alt_goal.y = (bonds_grid.y * 2 + 1) + goal_grid.y;
    }
    else if (goal_grid.y > 0)
    {
        // If bonds min/max is (-3,-3)(3,3) and goal is (x, 3), then alt goal is (-(3*2+1) + 3) = -4
        alt_goal.y = -(bonds_grid.y * 2 + 1) + goal_grid.y;
    }
    if (path_heuristic(start_grid, alt_goal) < path_heuristic(start_grid, goal_grid))
    {
        goal_grid = alt_goal;
    }

    auto priority_func = [](const std::pair<int, glm::ivec2>& a, const std::pair<int, glm::ivec2>& b) {
        return a.first > b.first;
    };

    std::priority_queue<std::pair<int, glm::ivec2>, std::vector<std::pair<int, glm::ivec2>>, decltype(priority_func)> open(
        priority_func);

    open.emplace(0.f, start_grid);
    a_star_grid[start_grid] = start_grid;
    a_star_cost[start_grid] = 0;

    while (!open.empty())
    {
        glm::ivec2 curr = open.top().second;
        open.pop();
        if (curr == goal_grid)
        {
            reconstruct_path(start_grid, goal_grid, goal_vec, poss, accuracy, a_star_grid);
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
                    a_star_cost[next] = new_cost;
                    a_star_grid[next] = curr;
                    int priority      = new_cost + path_heuristic(next, goal_grid);

                    open.emplace(priority, next);
                }
            }
        }
    }
    return false;
}
} // namespace cs::ai
