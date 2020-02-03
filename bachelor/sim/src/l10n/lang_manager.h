#include "filesystem.h"

#include <string>
#include <unordered_map>
#include <vector>

#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace cs
{
namespace lang
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
    LangManager(sol::state_view lua);

    /**
     * Creates a Language Manager to do localizations
     *
     * @param lua takes in a lua state_view to run lua script
     * @param locale initialize it with a language locale
     */
    LangManager(sol::state_view lua, const std::string& locale);

    /**
     * Changes the current locale/langauge we are running
     * @param locale the key of the langauge you want to change to
     */
    void set_locale(const std::string& locale);
    ~LangManager();
};
} // namespace lang
} // namespace cs
