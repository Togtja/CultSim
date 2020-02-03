#include "lang_manager.h"
namespace cs
{
namespace lang
{
LangManager::LangManager(sol::state_view lua) : m_lua(lua)
{
    set_locale("en");
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
{
}

LangManager::~LangManager()
{
}
} // namespace lang
} // namespace cs
