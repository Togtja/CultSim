#include "parsers.h"

#include <entt/meta/factory.hpp>

namespace cs::lua
{
ai::Strategy parse_strategy(const std::string& name, const sol::state_view lua)
{
    std::vector<action::Action> actions{};

    sol::table table = lua["strategies"][name];
    for (const auto& string : table["actions"].get<std::vector<std::string>>())
    {
        actions.push_back(parse_action(string, lua));
    }

    return {table["name"].get<std::string>(), 0.f, table["requirements"].get<ETag>(), table["tags"].get<ETag>(), actions};
}

action::Action parse_action(const std::string& name, const sol::state_view lua)
{
    sol::table table = lua["actions"][name];

    return {table["name"].get<std::string>(),
            table["requirements"].get<ETag>(),
            table["time_to_complete"].get<float>(),
            0.f,
            entt::null,
            table["success"].get<decltype(action::Action::success)>(),
            table["failure"].get<decltype(action::Action::failure)>(),
            {}};
}

} // namespace cs::lua
