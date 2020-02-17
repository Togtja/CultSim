#pragma once
#include "constants.h"
#include "robin_hood.h"

#include <queue>
#include <random>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using PathGrid = glm::ivec2;

glm::ivec2 world_to_grid(glm::vec2 pos, int grid = SIM_GRID_SIZE);

int path_heuristic(PathGrid start, PathGrid goal);

bool path_finding2(glm::vec2 start_vec, glm::vec2 goal_vec, std::vector<glm::vec3>& poss);