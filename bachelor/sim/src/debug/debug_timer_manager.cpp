#include "debug_timer_manager.h"

#include <gfx/ImGUI/imgui.h>

namespace cs
{
void DebugTimerManager::add_entry(std::string_view name, double time_ms)
{
    m_results[name] = time_ms;
}

void DebugTimerManager::show_debug_ui() const
{
    if (ImGui::TreeNode("Debug Timers"))
    {
        for (auto&& [k, v] : m_results)
        {
            ImGui::Text("%s: %.2lfms", k.data(), v);
        }
    }
}

DebugTimerManager& getDebugTimerResults()
{
    static auto dtm = DebugTimerManager{};
    return dtm;
}
} // namespace cs
