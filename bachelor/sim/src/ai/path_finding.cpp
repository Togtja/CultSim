#include "path_finding.h"

#include <spdlog/spdlog.h>

namespace cs::ai
{
std::vector<glm::ivec2> pos_to_wrap_grid(const glm::vec2 pos, const glm::vec2 bounds, const int grid_size)
{
    auto goal_grid   = world_to_grid(pos, grid_size);
    auto bounds_grid = world_to_grid(bounds, grid_size);
    std::vector<glm::ivec2> ret;

    glm::ivec2 alt_goal{goal_grid};
    if (goal_grid.x < 0)
    {
        // If bonds min/max is (-3,-3)(3,3) and goal is (-2, y), then alt goal is (3*2+1 + -2) = 5
        alt_goal.x = (bounds_grid.x * 2 + 1) + goal_grid.x;
    }
    else if (goal_grid.x > 0)
    {
        // If bonds min/max is (-3,-3)(3,3) and goal is (2, y), then alt goal is (-(3*2+1) + 2) = -5
        alt_goal.x = -(bounds_grid.x * 2 + 1) + goal_grid.x;
    }
    ret.emplace_back(alt_goal);

    glm::ivec2 alt_goal2{goal_grid};
    // Cheking vertical
    if (goal_grid.y < 0)
    {
        // If bonds min/max is (-3,-3)(3,3) and goal is (x, -3), then alt goal is (3*2+1 + -3) = 4
        alt_goal.y  = (bounds_grid.y * 2 + 1) + goal_grid.y;
        alt_goal2.y = (bounds_grid.y * 2 + 1) + goal_grid.y;
    }
    else if (goal_grid.y > 0)
    {
        // If bonds min/max is (-3,-3)(3,3) and goal is (x, 3), then alt goal is (-(3*2+1) + 3) = -4
        alt_goal.y  = -(bounds_grid.y * 2 + 1) + goal_grid.y;
        alt_goal2.y = -(bounds_grid.y * 2 + 1) + goal_grid.y;
    }

    ret.emplace_back(alt_goal);
    ret.emplace_back(alt_goal2);
    ret.emplace_back(goal_grid);

    return ret;
}

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
                     const glm::vec2& bounds,
                     const int accuracy)
{
    robin_hood::unordered_flat_map<glm::ivec2, glm::ivec2> a_star_grid{};
    robin_hood::unordered_flat_map<glm::ivec2, int> a_star_cost{};

    auto start_grid = world_to_grid(start_vec, accuracy);
    auto goal_grid  = world_to_grid(goal_vec, accuracy);

    auto priority_func = [](const std::pair<int, glm::ivec2>& a, const std::pair<int, glm::ivec2>& b) {
        return a.first > b.first;
    };

    std::priority_queue<std::pair<int, glm::ivec2>, std::vector<std::pair<int, glm::ivec2>>, decltype(priority_func)> open(
        priority_func);

    for (auto&& i : pos_to_wrap_grid(goal_vec, bounds, accuracy))
    {
        open.emplace(path_heuristic(start_grid, i), i);
    }

    goal_grid = open.top().second;

    // Priority_queue does not have a clear function
    while (!open.empty())
    {
        open.pop();
    }

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
