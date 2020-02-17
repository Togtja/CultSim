#include "filesystem/filesystem.h"

#include <string>
#include <vector>

#include <robin_hood.h>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace cs
{
/**
 * Class to maintain and cache all program strings intended to be observed by a user
 */
class Locale
{
private:
    robin_hood::unordered_map<std::string, std::string> m_strings;
    std::string m_lang;
    sol::state_view m_lua;

public:
    /**
     * Creates a Locale to do localizations
     *
     * @param lua takes in a lua state_view to be able to read lua state
     * @note Will default to the locale set in preferences
     */
    explicit Locale(sol::state_view lua);

    /**
     * Get a vector list of all avaliable langauges
     *
     * @note It checks for files in the l10n folder
     * @return returns a list of all languages
     */
    [[nodiscard]] std::vector<std::string> available_lang() const;

    /**
     * Changes the current locale/langauge we are running
     *
     * @param locale the key of the langauge you want to change to
     */
    void set_locale(const std::string& locale);

    /**
     * Get a string from the current locale with the given id
     *
     * @note id's and values for langauges can be found in l10n folder
     * @param id The id/key of the string you want
     * @return The value that belongs to the key, i.e. string in the locale language
     */
    std::string_view get_string(std::string_view id);

    /**
     * A short hand to get the current language, in English
     *
     * @return Name of language in english
     */
    std::string_view current_lang();
};
} // namespace cs
