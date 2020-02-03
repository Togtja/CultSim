#include "lang_manager.h"
namespace cs
{
namespace lang
{
LangManager::LangManager(sol::state_view lua) : m_lua(lua)
{
    set_locale("en");
}

LangManager::LangManager(sol::state_view lua, const std::string& locale) : m_lua(lua)
{
    set_locale(locale);
}

std::vector<std::string> LangManager::available_lang()
{
    return fs::list_directory("l10n");
}

void LangManager::set_locale(const std::string& locale)
{
    m_langs_map.clear();
    auto lua_lang = fs::read_file("l10n/" + locale + ".lua");

    m_lua.safe_script(lua_lang);
    sol::table lang_table = m_lua["locale"];

    for (auto it : lang_table)
    {
        m_langs_map.emplace(it.first.as<std::string>(), it.second.as<std::string>());
    }
    m_lang = m_langs_map.at("lang");
}

std::string_view LangManager::get_locale(std::string_view id)
{
    return m_langs_map.at(id.data());
}

std::string_view LangManager::current_lang()
{
    return m_lang;
}

LangManager::~LangManager()
{
}
} // namespace lang
} // namespace cs
