#pragma once
#include "constants.h"
#include "robin_hood.h"

#include <queue>
#include <random>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs::ai
{
glm::ivec2 world_to_grid(const glm::vec2& pos, int grid = SIM_GRID_SIZE);

int path_heuristic(glm::ivec2 start, glm::ivec2 goal);

void reconstruct_path(const glm::ivec2& start,
                      const glm::ivec2& grid_goal,
                      const glm::vec2& goal,
                      std::vector<glm::vec3>& pos,
                      const int accuracy,
                      const robin_hood::unordered_flat_map<glm::ivec2, glm::ivec2>& a_star_grid);

bool find_path_astar(const glm::vec2& start_vec,
                     const glm::vec2& goal_vec,
                     std::vector<glm::vec3>& poss,
                     const glm::vec2& bonds,
                     const int accuracy = 32);
} // namespace cs::ai
