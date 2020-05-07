#include "auto_timer.h"
#include "common_helpers.h"

#include <algorithm>
#include <array>

#include <gfx/ImGUI/imgui.h>
#include <glm/common.hpp>
#include <glm/vec4.hpp>

namespace cs
{
/** Variables for coloring the text output */
static constexpr auto s_color_good_time = glm::vec4{0.2f, 1.f, 0.66f, 1.f};
static constexpr auto s_color_bad_time  = glm::vec4{1.f, 0.4f, 0.2f, 1.f};
static constexpr std::array<glm::vec4, 2> s_time_color_coding{s_color_good_time, s_color_bad_time};

void AutoTimer::show_debug_ui()
{
    if (ImGui::BeginTable("Debug Timers", 2, ImGuiTableFlags_BordersOuter))
    {
        ImGui::TableSetupColumn("Timer ID");
        ImGui::TableSetupColumn("Time (ms)");
        cs_auto_table_headers();

        std::sort(s_results.begin(), s_results.end(), [](const auto& a, const auto& b) { return a.time_ms > b.time_ms; });
        for (const auto& k : s_results)
        {
            ImGui::TableNextCell();
            ImGui::Text("%s", k.name.data());
            ImGui::TableNextCell();

            /** Show time in MS colored from green to red based on how long it took relative to 60 FPS */
            ImGui::TextColored(glm::mix(s_time_color_coding[0], s_time_color_coding[1], glm::vec4(k.time_ms / 16.f)),
                               "%.3lf",
                               k.time_ms);
        }
        ImGui::EndTable();
    }
    s_results.clear();
}
} // namespace cs
