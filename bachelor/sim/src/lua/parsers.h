#pragma once

#include "entity/components/strategy.h"

#include <sol/table.hpp>

namespace cs::lua
{
ai::Strategy parse_strategy(const sol::table& table);
}
