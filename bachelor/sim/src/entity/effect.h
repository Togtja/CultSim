#pragma once
#include "components/components.h"

namespace cs::effect
{
void affect_traits(entt::entity e, const component::Traits& traits);
void unaffect_traits(entt::entity e, const component::Traits& traits);
} // namespace cs::effect
