#pragma once

#include <vector>

#include <sol/state_view.hpp>
#include <robin_hood.h>

namespace cs
{
class NameGenerator
{
private:
    /**
     * Holds available names for a specific ethnicity
     */
    struct Ethnicity
    {
        std::vector<std::string> male_names{};
        std::vector<std::string> female_names{};
        std::vector<std::string> surnames{};
    };

    /** Names available for generation */
    robin_hood::unordered_map<std::string, Ethnicity> m_names{};

public:
    NameGenerator(sol::state_view lua, const std::string& name_table);
};

} // namespace cs
