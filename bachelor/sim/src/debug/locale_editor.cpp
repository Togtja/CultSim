#include "locale_editor.h"
#include "common_helpers.h"
#include "filesystem/filesystem.h"
#include "l10n/lang_manager.h"

#include <algorithm>
#include <utility>

#include <gfx/ImGUI/imgui.h>

namespace cs
{
/** TODO: Add comments?*/

void LocaleEditor::init(sol::state_view lua)
{
    auto locales = fs::list_directory("l10n");
    for (const auto& str : locales)
    {
        auto script_file = fs::read_file("l10n/" + str);
        lua.script(script_file);
        sol::table locale = lua["locale"];

        m_locales.push_back(str);
        for (auto&& [k, v] : locale)
        {
            m_translations[k.as<std::string>()].push_back(v.as<std::string>());
        }
    }

    for (auto&& [k, v] : m_translations)
    {
        v.resize(m_locales.size());
        for (auto&& elem : v)
        {
            elem.resize(MAX_STR_LEN);
        }
    }
}

/** TODO: Add Comments? */

void LocaleEditor::show()
{
    ImGui::Begin("Locale Editor");

    ImGui::BeginTable("String Table##Locales", m_locales.size() + 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersH);
    ImGui::TableSetupColumn("String ID");
    for (const auto& locale : m_locales)
    {
        ImGui::TableSetupColumn(locale.c_str());
    }
    cs_auto_table_headers();

    int cell = 0;
    for (auto&& [k, v] : m_translations)
    {
        ImGui::TableNextCell();
        ImGui::Text("%s", k.c_str());
        for (auto&& translation : v)
        {
            ImGui::TableNextCell();
            ImGui::PushID(++cell);
            ImGui::InputText("##CELL", translation.data(), MAX_STR_LEN);
            ImGui::PopID();
        }
    }
    ImGui::EndTable();

    ImGui::End();
}
}; // namespace cs
