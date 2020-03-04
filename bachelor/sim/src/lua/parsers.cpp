#include "parsers.h"

#include <entt/meta/factory.hpp>

namespace cs
{
ai::Strategy lua::parse_strategy(const sol::table& table)
{
    std::vector<action::Action> actions{};

    table["actions"].get<std::vector<action::Action>>()

        return {
            table["name"].get<std::string>(),
            0.f,
            table["requirements"].get<ETag>(),
            table["tags"].get<ETag>(),

        };
}

action::Action lua::parse_action(const sol::table& table)
{
}

} // namespace cs
