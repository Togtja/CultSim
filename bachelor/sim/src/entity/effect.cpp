#include "effect.h"

namespace cs::effect
{
void affect_personality(entt::entity e, const component::Personalities& personalities)
{
    for (const auto& i : personalities.personalities)
    {
        i.affect(e);
    }
}

void unaffect_personality(entt::entity e, const component::Personalities& personalities)
{
    for (const auto& i : personalities.personalities)
    {
        i.remove_affect(e);
    }
}
} // namespace cs::effect