#pragma once

#include <vector>

#include <robin_hood.h>
#include <sol/state_view.hpp>

namespace cs
{
class LocaleEditor
{
private:
    inline static constexpr int MAX_STR_LEN = 128;

    sol::state_view m_lua;
    std::vector<std::string> m_locales{};
    robin_hood::unordered_flat_map<std::string, std::vector<std::string>> m_translations{};

public:
    explicit LocaleEditor(sol::state_view lua);

    void show();
};
} // namespace cs
