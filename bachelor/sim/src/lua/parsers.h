#pragma once

#include "entity/components/strategy.h"

#include <sol/state_view.hpp>

namespace cs::lua
{
ai::Strategy parse_strategy(const std::string& name, const sol::state_view lua);

action::Action parse_action(const std::string& name, const sol::state_view lua);
} // namespace cs::lua
