#include "effect.h"

namespace cs::effect
{
void affect_traits(entt::entity e, const component::Traits& traits)
{
    for (const auto& i : traits.default_traits)
    {
        i.affect(e);
    }
}

void unaffect_traits(entt::entity e, const component::Traits& traits)
{
    for (const auto& i : traits.default_traits)
    {
        i.remove_affect(e);
    }
}
} // namespace cs::effect