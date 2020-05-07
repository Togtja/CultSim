#pragma once

#include <vector>

#include <robin_hood.h>
#include <sol/state_view.hpp>

namespace cs
{
class RandomEngine;

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
    /**
     * Initialize name generator with a table
     *
     * @param name_table
     */
    void initialize(sol::table name_table);

    /** TODO: Use in name component */
    /** A generated name */
    struct Name
    {
        std::string first{};
        std::string last{};
    };

    /**
     * Generate a name from the given ethnicity
     *
     * @param ethnicity The ethnicity to get a name from
     * @param male The sex of the name
     * @param rng The random generator to use for generation
     * @return The generated name split in first and last
     */
    Name generate(const std::string& ethnicity, bool male, RandomEngine& rng);
};

} // namespace cs
