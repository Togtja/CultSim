#pragma once
#include "constants.h"
#include "robin_hood.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs::ai
{
/**
 * Takes in a position and figure out its alternative location when wrapping
 *
 * Say in a 5x5 square you have a postion in the left bottom corner (0,0),
 * then this code would take that and find the 5 corner that would wrap to 0,0
 * this would be (5,0), (0,5) and (5,5), because if you "enter" one of the edges you get wrapped back again
 *
 * @param pos The position you want to wrap
 * @param bounds The bonds of the map
 * @param grid_size The size of the grids on the map
 * @return 4 positions inside a vector, 3 are the corner of the original position, the last is just the original
 */
std::vector<glm::vec2> pos_to_wrap_grid(const glm::vec2& pos, const glm::vec2& bounds, const int grid_size);

/**
 * Given a world position return the gird it belongs too
 *
 * @param pos The position you want to check
 * @param grid The size of the grid
 * @return A int vector of what grid it belongs to
 */
glm::ivec2 world_to_grid(const glm::vec2& pos, const int grid);

/**
 * Given a world position, and a bounds return the grid it belongs too
 *
 * @note wraps the position based of the bounds
 *
 * @param pos The position you want to check
 * @param grid The size of the grid
 * @param bounds The bounds of the map in world position
 * @return glm::ivec2
 */
glm::ivec2 world_to_grid_bound(const glm::vec2& pos, const int grid, const glm::ivec2& bounds);

/**
 * Given a start and a goal location, finds the heuristic cost between them, and returns it
 *
 * @param start The start position
 * @param goal The goal position
 * @return The heuristic cost to move between them
 */
int path_heuristic(const glm::ivec2& start, const glm::ivec2& goal);

/**
 * Takes in a start gird position, and a goal grid position and reconstruct the path using the A* grid
 *
 * @note this is only used as a subfunction to the find_path_astar function
 *
 * @param start The start grid position
 * @param grid_goal The goal grid possition
 * @param goal The actual goal (not just the grid)
 * @param pos A Vector where the new path get placed in world position
 * @param accuracy The accuracy of the pathfinding
 * @param a_star_grid The A* grid of paths that has been constructed
 */
void reconstruct_path(const glm::ivec2& start,
                      const glm::ivec2& grid_goal,
                      const glm::vec2& goal,
                      std::vector<glm::vec3>& pos,
                      const int accuracy,
                      const robin_hood::unordered_flat_map<glm::ivec2, glm::ivec2>& a_star_grid);

/**
 * Given a start position and a goal position, finds a path between them, and fills it in the pos argument.
 *
 * @param start_vec The start position
 * @param goal_vec The goal position
 * @param pos The vector that the path fill up
 * @param bounds The bounds of the map
 * @param accuracy Accuracy of the pathfinding, typically grid_size, but could be smaller for more accurate movement
 * @return True if it finds a path, otherwise false
 */
bool find_path_astar(const glm::vec2& start_vec,
                     const glm::vec2& goal_vec,
                     std::vector<glm::vec3>& pos,
                     const glm::vec2& bounds,
                     const int accuracy = SIM_GRID_SIZE);
} // namespace cs::ai
