#include "traits.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Traits::update(float dt)
{
    CS_AUTOTIMER(Traits System);
    auto view = m_context.registry->view<component::Traits>();
    view.each([this](entt::entity e, component::Traits& traits) {
        /** Trying to get traits */
        for (const auto& trait : traits.attainable_traits)
        {
            /** As long as the attainable trait is not already acquired */
            const auto& it = std::find(traits.acquired_traits.begin(), traits.acquired_traits.end(), trait);
            if (it == traits.acquired_traits.end())
            {
                /* Check if we satisfy the user typed requirement */
                if (trait.attain(e))
                {
                    trait.affect(e);
                    traits.acquired_traits.push_back(trait);
                }
            }
        }

        /* Add traits to be deleted (to avoid deleting in loop) */
        std::vector<component::detail::Trait> to_delete;
        for (const auto& trait : traits.acquired_traits)
        {
            if (trait.lose(e))
            {
                trait.remove_affect(e);
                to_delete.push_back(trait);
            }
        }

        /* Delete the traits we found */
        for (const auto& trait : to_delete)
        {
            const auto& it = std::find(traits.acquired_traits.begin(), traits.acquired_traits.end(), trait);
            if (it != traits.acquired_traits.end())
            {
                traits.acquired_traits.erase(it);
            }
        }
    });
}

ISystem* Traits::clone()
{
    return new Traits(m_context);
}
} // namespace cs::system
