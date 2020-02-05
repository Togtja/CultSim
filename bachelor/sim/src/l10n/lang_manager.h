#include "filesystem/filesystem.h"

#include <string>
#include <unordered_map>
#include <vector>

#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace cs::lang
{
class LangManager
{
private:
    std::unordered_map<std::string, std::string> m_langs_map;
    std::string m_lang;
    sol::state_view m_lua;

public:
    /**
     * Creates a Language Manager to do localizations
     *
     * @param lua takes in a lua state_view to run lua script
     */
    explicit LangManager(sol::state_view lua);

    /**
     * Creates a Language Manager to do localizations
     *
     * @param lua takes in a lua state_view to run lua script
     * @param locale initialize it with a language locale
     */
    LangManager(sol::state_view lua, const std::string& locale);

    /**
     * Get a vector list of all avaliable langauges
     *
     * @note It checks for files in the l10n folder
     * @return returns a list of all languages
     */
    std::vector<std::string> available_lang();

    /**
     * Changes the current locale/langauge we are running
     *
     * @param locale the key of the langauge you want to change to
     */
    void set_locale(const std::string& locale);

    /**
     * Get a locale given an id
     *
     * @note id's and values for langauges can be found in l10n folder
     * @param id The id/key of the string you want
     * @return The value that belongs to the key, i.e. string in the locale language
     */
    std::string_view get_locale(std::string_view id);

    /**
     * A short hand to get the current language, in English
     *
     * @return Name of language in english
     */
    std::string_view current_lang();
};
} // namespace cs
