#pragma once

#include <cstdint>

#include <entt/entity/registry.hpp>
#include <gfx/ImGUI/imgui.h>
#include <gfx/ImGUI/imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <sol/state_view.hpp>

namespace cs
{
class SceneManager;
class PreferenceManager;

/**
 * The application context struct give other sections of the program access to application wide data
 */
struct ApplicationContext
{
    sol::state_view lua_state;
    SceneManager* scene_manager{nullptr};
    PreferenceManager* preferences{nullptr};
};

struct Notification
{
    std::string title{};
    std::string information{};
    float time_shown{};
};

/**
 * Helper struct for entity selection
 *
 * Used as a context variable within registries in the scenario scene
 */
struct EntitySelectionHelper
{
    entt::entity hovered_entity{entt::null};
    entt::entity selected_entity{entt::null};
};
/**
 * Count number of set bits in a uint
 *
 * @param i The integer to count
 * @return The number of set bits in the integer
 */
unsigned count_set_bits(uint64_t i);

/**
 * Check if two positions are within the threshold of each other
 *
 * @param pos The first position
 * @param pos2 The second position
 * @param threshold The distance within which the two positions are considered close enough
 * @return True if pos and pos2 are close enough to each other
 */
bool close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold);

/**
 * Computes the sigmoid function of x
 *
 * @param i The number to compute on
 * @return float The sigmoid
 */
float sigmoid(const float i);

/**
 * Creates headers for ImGui tables like ImGui::AutoTableHeaders, except with non-clickable or hoverable cells
 */
void cs_auto_table_headers();
} // namespace cs
