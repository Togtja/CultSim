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

inline unsigned count_set_bits(uint64_t x)
{
    unsigned sum{};
    while (x != 0)
    {
        sum += x & 1u;
        x >>= 1u;
    }
    return sum;
}

inline bool close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold)
{
    glm::bvec2 boolvec = glm::epsilonEqual(pos, pos2, glm::vec2(threshold));
    return boolvec.x && boolvec.y;
}

inline float sigmoid(const float x)
{
    return exp(x) / (exp(x) + 1.f);
}

inline void cs_auto_table_headers()
{
    auto& g         = *ImGui::GetCurrentContext();
    int rowCount    = g.CurrentTable->ColumnsCount;
    auto row_height = ImGui::GetTextLineHeight();
    ImGui::TableNextRow(ImGuiTableRowFlags_Headers, row_height);
    for (int i = 0; i < rowCount; i++)
    {
        auto name = ImGui::TableGetColumnName(i);
        // ImGui::TableHeader(name);
        ImGui::Text("%s", name);
        if (i < rowCount - 1)
        {
            ImGui::TableNextCell();
        }
    }
}
} // namespace cs
