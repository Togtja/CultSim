#include "preferences.h"
#include "constants.h"
#include "filesystem/filesystem.h"

#include <gfx/ImGUI/imgui.h>
#include <glad/glad.h>
#include <spdlog/fmt/bundled/format.h>

namespace cs
{
PreferenceManager::PreferenceManager(Window& window, sol::state_view lua_state) :
    m_window(window),
    m_locale(lua_state),
    m_lua(lua_state)
{
}

PreferenceManager::PreferenceManager(Window& window, sol::state_view lua_state, std::string_view from_file) :
    m_window(window),
    m_locale(lua_state),
    m_lua(lua_state)
{
}

void PreferenceManager::show_debug_ui()
{
    static bool fs = std::get<bool>(m_fullscreen.value);

    ImGui::Begin("Preferences");

    if (ImGui::Checkbox(m_fullscreen.name.c_str(), &fs))
    {
        set_fullscreen(fs);
    }

    ImGui::End();
}

void PreferenceManager::init()
{
    load_from_lua();
    m_locale.set_locale(std::get<std::string>(m_language.value));
}

void PreferenceManager::deinit()
{
    save_to_lua();
}

const Preference& PreferenceManager::get_resolution() const
{
    return m_resolution;
}

void PreferenceManager::set_resolution(const glm::ivec2& resolution)
{
    auto old = m_resolution;
    m_window.resize(resolution);
    m_resolution.value = resolution;
    m_preference_changed.publish(old, m_resolution);
}

const Preference& PreferenceManager::get_fullscreen() const
{
    return m_fullscreen;
}

void PreferenceManager::set_fullscreen(bool fullscreen)
{
    auto old = m_fullscreen;
    m_window.set_fullscreen(fullscreen);
    m_fullscreen.value = fullscreen;
    m_preference_changed.publish(old, m_fullscreen);
}

const Preference& PreferenceManager::get_language() const
{
    return m_language;
}

void PreferenceManager::set_language(std::string_view language)
{
    auto old = m_language;
    m_locale.set_locale(std::string(language));
    m_language.value = std::string(language);
    m_preference_changed.publish(old, m_language);
}

std::string_view PreferenceManager::get_string(std::string_view id)
{
    return m_locale.get_string(id);
}

void PreferenceManager::load_from_lua()
{
    const auto script = fs::read_file(PATH_PREFERENCE_FILE);
    m_lua.script(script);

    sol::table preferences = m_lua["preferences"];
    set_fullscreen(preferences.get<bool>("Fullscreen"));
    set_language(preferences.get<std::string>("Language"));
    set_resolution({preferences.get<std::vector<int>>("Resolution")[0], preferences.get<std::vector<int>>("Resolution")[1]});
}

std::string PreferenceManager::write_preference(const Preference& preference)
{
    /* clang-format off */
    return std::visit(Overloaded {
        [&preference](auto arg) { return fmt::format("\t{} = {},\n", preference.name, arg); },
        [&preference](std::string arg) { return fmt::format("\t{} = \"{}\",\n", preference.name, arg); },
        [&preference](glm::ivec2 arg) { return fmt::format("\t{} = {{{}, {}}},\n", preference.name, arg.x, arg.y); }},
        preference.value);
    /* clang-format on */
}

void PreferenceManager::save_to_lua()
{
    auto output = std::string("preferences = {\n");
    output += write_preference(m_resolution);
    output += write_preference(m_fullscreen);
    output += write_preference(m_language);
    output += "}\n";

    fs::write_file(PATH_PREFERENCE_FILE, output);
}
} // namespace cs
