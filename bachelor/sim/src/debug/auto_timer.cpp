#include "auto_timer.h"

#include <gfx/ImGUI/imgui.h>

namespace cs
{
void AutoTimer::show_debug_ui()
{
    if (ImGui::TreeNode("Debug Timers"))
    {
        for (auto&& [k, v] : s_results)
        {
            ImGui::Text("%s: %.2lfms", k.data(), v);
        }
    }
}
} // namespace cs
