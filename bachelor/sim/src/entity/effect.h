#pragma once
#include "components/components.h"

namespace cs::effect
{
void affect_personality(entt::entity e, const component::Personalities& personalities);
void unaffect_personality(entt::entity e, const component::Personalities& personalities);
} // namespace cs::effect
