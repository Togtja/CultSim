#include "auto_timer.h"

#include <algorithm>

#include <gfx/ImGUI/imgui.h>

namespace cs
{
void AutoTimer::show_debug_ui()
{
    if (ImGui::BeginTable("Debug Timers", 2, ImGuiTableFlags_BordersOuter))
    {
        ImGui::TableSetupColumn("Timer ID");
        ImGui::TableSetupColumn("Time (ms)");
        ImGui::TableAutoHeaders();

        std::sort(s_results.begin(), s_results.end(), [](auto&& a, auto&& b) { return a.time_ms > b.time_ms; });
        for (const auto& k : s_results)
        {
            ImGui::TableNextCell();
            ImGui::Text("%s", k.name.data());
            ImGui::TableNextCell();
            ImGui::TextColored({0.3f, 1.f, 0.66f, 1.f}, "%.2lf", k.time_ms);
        }
        ImGui::EndTable();
    }
    s_results.clear();
}
} // namespace cs
