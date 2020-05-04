#include "path_finding.h"

#include <cmath>
#include <queue>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <spdlog/spdlog.h>

namespace cs::ai
{

std::vector<glm::vec2> pos_to_wrap_grid(const glm::vec2& pos, const glm::vec2& bounds, const int grid_size)
{

    glm::vec2 alt_xpos{pos};
    if (pos.x < 0)
    {
        /** If bonds min/max is (-3,-3)(3,3) and goal is (-2, y), then alt goal is (3*2+1 + -2) = 5 */
        alt_xpos.x = (bounds.x * 2 + grid_size) + pos.x;
    }
    else if (pos.x > 0)
    {
        /** If bonds min/max is (-3,-3)(3,3) and goal is (2, y), then alt goal is (-(3*2+1) + 2) = -5 */
        alt_xpos.x = -(bounds.x * 2 + grid_size) + pos.x;
    }

    glm::vec2 alt_ypos{pos};
    /** Cheking vertical */
    if (pos.y < 0)
    {
        /** If bonds min/max is (-3,-3)(3,3) and goal is (x, -3), then alt goal is (3*2+1 + -3) = 4 */
        alt_ypos.y = (bounds.y * 2 + grid_size) + pos.y;
    }
    else if (pos.y > 0)
    {
        /** If bonds min/max is (-3,-3)(3,3) and goal is (x, 3), then alt goal is (-(3*2+1) + 3) = -4 */
        alt_ypos.y = -(bounds.y * 2 + grid_size) + pos.y;
    }

    return {alt_xpos,alt_ypos, glm::vec2(alt_xpos.x, alt_ypos.y), pos};
}

glm::ivec2 world_to_grid(const glm::vec2& pos, const int grid)
{
    return {static_cast<int>(std::floor(pos.x / grid)), static_cast<int>(std::floor(pos.y / grid))};
}

/** TODO: Add comments explaining what is happening and why */
glm::ivec2 world_to_grid_bound(const glm::vec2& pos, const int grid, const glm::ivec2& bounds)
{
    int nx = std::floor((pos.x - bounds.x) / grid);
    int mx = std::floor((2 * bounds.x) / grid);

    int ny = std::floor((pos.y - bounds.y) / grid);
    int my = std::floor((2 * bounds.y) / grid);
    return {(((nx % mx) + mx) % mx) - (bounds.x / grid), (((ny % my) + my) % my) - (bounds.y / grid)};
}

int path_heuristic(const glm::ivec2& start, const glm::ivec2& goal)
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
                     std::vector<glm::vec3>& pos,
                     const glm::vec2& bounds,
                     const int accuracy)
{
    robin_hood::unordered_flat_map<glm::ivec2, glm::ivec2> a_star_grid{};
    robin_hood::unordered_flat_map<glm::ivec2, int> a_star_cost{};

    auto start_grid = world_to_grid(start_vec, accuracy);
    auto goal_grid  = world_to_grid(goal_vec, accuracy);

    /** TODO: Comment as to why this is necessary */
    auto priority_func = [](const std::pair<int, glm::ivec2>& a, const std::pair<int, glm::ivec2>& b) {
        return a.first > b.first;
    };

    std::priority_queue<std::pair<int, glm::ivec2>, std::vector<std::pair<int, glm::ivec2>>, decltype(priority_func)> open(
        priority_func);

    for (const auto& i : pos_to_wrap_grid(goal_vec, bounds, accuracy))
    {
        auto grid_pos = world_to_grid(i, accuracy);
        open.emplace(path_heuristic(start_grid, grid_pos), grid_pos);
    }

    goal_grid = open.top().second;
    
    /** Priority_queue does not have a clear function */
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
            reconstruct_path(start_grid, goal_grid, goal_vec, pos, accuracy, a_star_grid);
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
