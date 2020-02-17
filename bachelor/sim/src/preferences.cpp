#include "preferences.h"
#include "filesystem/filesystem.h"

#include <gfx/ImGUI/imgui.h>
#include <glad/glad.h>

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
    /** TODO: Interact with Locale, or integrate it */
    m_language.value = std::string(language);
}
} // namespace cs
