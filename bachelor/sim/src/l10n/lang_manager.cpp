#include "lang_manager.h"

#include <utility>

namespace cs
{
/** TODO: Use this */
Locale::Locale(sol::state_view lua) : m_lua(std::move(lua))
{
}

std::vector<std::string> Locale::available_lang() const
{
    return fs::list_directory("l10n");
}

void Locale::set_locale(const std::string& locale)
{
    m_strings.clear();
    const auto lua_lang = fs::read_file("l10n/" + locale + ".lua");

    m_lua.script(lua_lang);
    sol::table lang_table = m_lua["locale"];

    for (auto&& [k, v] : lang_table)
    {
        m_strings.emplace(k.as<std::string>(), v.as<std::string>());
    }

    m_lang = m_strings.at("lang");
}

std::string_view Locale::get_string(std::string_view id)
{
    return m_strings.at(id.data());
}

std::string_view Locale::current_lang()
{
    return m_lang;
}

} // namespace cs
