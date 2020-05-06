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
 * TODO: Document
 *
 * @param x
 * @return unsigned
 */
unsigned count_set_bits(uint64_t x);

/**
 * TODO: Document
 *
 * @param pos
 * @param pos2
 * @param threshold
 * @return true
 * @return false
 */
bool close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold);

/**
 * TODO: Document
 *
 * @param x
 * @return float
 */
float sigmoid(const float x);

/**
 * TODO: Document
 *
 */
void cs_auto_table_headers();
} // namespace cs
