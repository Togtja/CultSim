#include "name_generator.h"
#include "random_engine.h"

namespace cs
{
void NameGenerator::initialize(sol::table name_table)
{
    sol::table ethnicity_table = name_table["ethnicities"];
    for (const auto& [k, v] : ethnicity_table)
    {
        sol::table names             = v.as<sol::table>();
        m_names[k.as<std::string>()] = Ethnicity{names["male_first"].get<std::vector<std::string>>(),
                                                 names["female_first"].get<std::vector<std::string>>(),
                                                 names["surnames"].get<std::vector<std::string>>()};
    }
}

NameGenerator::Name NameGenerator::generate(const std::string& ethnicity, bool male, RandomEngine& rng)
{
    const auto& nameref = (male ? m_names.at(ethnicity).male_names : m_names.at(ethnicity).female_names);
    return {rng.pick(nameref), rng.pick(m_names.at(ethnicity).surnames)};
}
} // namespace cs
