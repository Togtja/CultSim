#include "auto_timer.h"
#include "common_helpers.h"

#include <algorithm>
#include <array>

#include <gfx/ImGUI/imgui.h>
#include <glm/common.hpp>
#include <glm/vec4.hpp>

namespace cs
{
static constexpr std::array<glm::vec4, 2> s_ms_col{glm::vec4{0.2f, 1.f, 0.66f, 1.f}, glm::vec4{1.f, 0.4f, 0.2f, 1.f}};

void AutoTimer::show_debug_ui()
{
    if (ImGui::BeginTable("Debug Timers", 2, ImGuiTableFlags_BordersOuter))
    {
        ImGui::TableSetupColumn("Timer ID");
        ImGui::TableSetupColumn("Time (ms)");
        cs_auto_table_headers();

        std::sort(s_results.begin(), s_results.end(), [](auto&& a, auto&& b) { return a.time_ms > b.time_ms; });
        for (const auto& k : s_results)
        {
            ImGui::TableNextCell();
            ImGui::Text("%s", k.name.data());
            ImGui::TableNextCell();
            ImGui::TextColored(glm::mix(s_ms_col[0], s_ms_col[1], glm::vec4(k.time_ms / 16.f)), "%.3lf", k.time_ms);
        }
        ImGui::EndTable();
    }
    s_results.clear();
}
} // namespace cs
