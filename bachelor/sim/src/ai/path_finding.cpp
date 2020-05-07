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

    return {alt_xpos, alt_ypos, glm::vec2(alt_xpos.x, alt_ypos.y), pos};
}

glm::ivec2 world_to_grid(const glm::vec2& pos, const int grid)
{
    return {static_cast<int>(std::floor(pos.x / grid)), static_cast<int>(std::floor(pos.y / grid))};
}

glm::ivec2 world_to_grid_bound(const glm::vec2& pos, const int grid, const glm::ivec2& bounds)
{
    /**
     * In Python it would look something this:
     * ((pos - bounds)/grid)%(2*bounds/grid) - (bounds/grid)
     * This makes it belong to a grid while a pos is between 2 grid values
     * But also wrap grid if it is out of bounds
     *
     * but due do the fact that in C/C++ -1%9 = -1
     * To translate this we need to do ((n % M) + M) % M
     * so ((-1%9) + 9) % 9 = 1
     */

    int nx = static_cast<int>(std::floor((pos.x - bounds.x) / grid));
    int mx = static_cast<int>(std::floor((2 * bounds.x) / grid));

    int ny = static_cast<int>(std::floor((pos.y - bounds.y) / grid));
    int my = static_cast<int>(std::floor((2 * bounds.y) / grid));

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

    /** Creates a priority function to sort path based on their heuristic level */
    auto priority_func = [](const std::pair<int, glm::ivec2>& a, const std::pair<int, glm::ivec2>& b) {
        return a.first > b.first;
    };
    /** A priority queue of pairs, where the first value represent the heuristic, and second value the location */
    std::priority_queue<std::pair<int, glm::ivec2>, std::vector<std::pair<int, glm::ivec2>>, decltype(priority_func)> open(
        priority_func);

    /** Wrap the goal position to the 4 possible locations and add them to the queue to figure out which goal is closest */
    for (const auto& i : pos_to_wrap_grid(goal_vec, bounds, accuracy))
    {
        auto grid_pos = world_to_grid(i, accuracy);
        open.emplace(path_heuristic(start_grid, grid_pos), grid_pos);
    }

    /** Set the closest goal as the goal grid */
    goal_grid = open.top().second;

    /** Priority queue does not have a clear function */
    while (!open.empty())
    {
        open.pop();
    }
    /** Place the start grid in the priority queue */
    open.emplace(0.f, start_grid);
    a_star_grid[start_grid] = start_grid;
    a_star_cost[start_grid] = 0;

    while (!open.empty())
    {
        /** We will visit the closest neighbour/node found via A* from the priority queue */
        glm::ivec2 curr = open.top().second;
        open.pop();

        /** If we have reach the goal using our A* we reconstruct the path, and report a succsess */
        if (curr == goal_grid)
        {
            reconstruct_path(start_grid, goal_grid, goal_vec, pos, accuracy, a_star_grid);
            return true;
        }

        glm::ivec2 next{};
        const auto max = glm::ivec2{curr.x + 1, curr.y + 1};
        const auto min = glm::ivec2{curr.x - 1, curr.y - 1};

        /** Find the neighbour grid that we will potentially visit next bases on a 3x3*/
        for (int x = min.x; x <= max.x; x++)
        {
            for (int y = min.y; y <= max.y; y++)
            {
                next.x = x;
                next.y = y;

                /** Make sure we don't visit ourself */
                if (next == curr)
                {
                    continue;
                }

                /** Calculate the cost */
                const int new_cost = a_star_cost[curr] + 1; // + cost of graph node needs to be computed
                if (a_star_cost.find(next) == a_star_cost.end() || new_cost < a_star_cost[next])
                {
                    a_star_cost[next]  = new_cost;
                    a_star_grid[next]  = curr;
                    const int priority = new_cost + path_heuristic(next, goal_grid);

                    open.emplace(priority, next);
                }
            }
        }
    }
    return false;
}
} // namespace cs::ai
