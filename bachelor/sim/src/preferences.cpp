#include "preferences.h"

namespace cs
{
PreferenceManager::PreferenceManager(Window& window) : m_window(window)
{
}

PreferenceManager::PreferenceManager(Window& window, std::string_view from_file) : m_window(window)
{
}

const Preference& PreferenceManager::get_resolution() const
{
    return m_resolution;
}

void PreferenceManager::set_resolution(const glm::ivec2& resolution)
{
    m_resolution.value = resolution;
}

const Preference& PreferenceManager::get_fullscreen() const
{
    return m_fullscreen;
}

void PreferenceManager::set_fullscreen(bool fullscreen)
{
    m_fullscreen.value = fullscreen;
}

const Preference& PreferenceManager::get_language() const
{
    return m_language;
}

void PreferenceManager::set_language(std::string_view language)
{
    m_language.value = std::string(language);
}
} // namespace cs
