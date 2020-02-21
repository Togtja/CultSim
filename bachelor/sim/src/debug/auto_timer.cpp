#include "auto_timer.h"

#include <gfx/ImGUI/imgui.h>

namespace cs
{
void AutoTimer::show_debug_ui()
{
    ImGui::BeginTable("Debug Timers", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Sortable);
    ImGui::TableSetupColumn("Timer");
    ImGui::TableSetupColumn("Time (ms)", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_PreferSortDescending);
    ImGui::TableAutoHeaders();
    for (auto&& [k, v] : s_results)
    {
        ImGui::TableNextCell();
        ImGui::Text("%s", k.data());
        ImGui::TableNextCell();
        ImGui::TextColored({0.3f, 1.f, 0.66f, 1.f}, "%.2lf", v);
    }
    ImGui::EndTable();
}
} // namespace cs
