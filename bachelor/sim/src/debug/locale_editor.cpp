#include "locale_editor.h"
#include "common_helpers.h"
#include "filesystem/filesystem.h"
#include "l10n/lang_manager.h"

#include <algorithm>
#include <utility>

#include <gfx/ImGUI/imgui.h>

namespace cs
{
void LocaleEditor::init(sol::state_view lua)
{
    /** Get the diffrent locales */
    auto locales = fs::list_directory("l10n");
    for (const auto& str : locales)
    {
        /** Read each of them and get the lua script/table*/
        auto script_file = fs::read_file("l10n/" + str);
        lua.script(script_file);
        sol::table locale = lua["locale"];

        m_locales.push_back(str);

        /** For each key -> string pair of the lua locale table: */
        for (auto&& [k, v] : locale)
        {
            /** Add it to the translation map */
            m_translations[k.as<std::string>()].push_back(v.as<std::string>());
        }
    }

    /** Add enough space/memory for UI editing */
    for (auto&& [k, v] : m_translations)
    {
        v.resize(m_locales.size());
        for (auto&& elem : v)
        {
            elem.resize(MAX_STR_LEN);
        }
    }
}

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
