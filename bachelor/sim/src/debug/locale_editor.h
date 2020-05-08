#pragma once

#include <vector>

#include <robin_hood.h>
#include <sol/state_view.hpp>

namespace cs
{
/**
 * Simple GUI editor to edit strings in the locale
 */
class LocaleEditor
{
private:
    /** Max length of a string in the locale */
    inline static constexpr int MAX_STR_LEN{128};

    /** The available locales */
    std::vector<std::string> m_locales{};

    /** The translations of a string key in the various locales */
    robin_hood::unordered_map<std::string, std::vector<std::string>> m_translations{};

public:
    /**
     * Initialize existing strings from the locale files
     *
     * @param lua The lua state holding the locale data
     */
    void init(sol::state_view lua);

    /**
     * Show the locale editor
     */
    void show();
};
} // namespace cs
