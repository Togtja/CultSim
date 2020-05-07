#include "effect.h"

namespace cs::effect
{
void affect_traits(entt::entity e, const component::Traits& traits)
{
    for (const auto& trait : traits.acquired_traits)
    {
        trait.affect(e);
    }
}

void unaffect_traits(entt::entity e, const component::Traits& traits)
{
    for (const auto& trait : traits.acquired_traits)
    {
        trait.unaffect(e);
    }
}
} // namespace cs::effect
