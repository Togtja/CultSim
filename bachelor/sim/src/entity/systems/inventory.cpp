#include "inventory.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include "spdlog/spdlog.h"

namespace cs::system
{
void Inventory::update(float dt)
{
    CS_AUTOTIMER(Inventory System);

    auto& registry = *m_context.registry;
    auto view     = registry.view<component::Inventory>();

    view.each([this, dt, &registry](entt::entity e, component::Inventory& inventory) {
        if (inventory.size != inventory.contents.size())
        {
            inventory.size = inventory.contents.size();
            ETag tags{};
            for (entt::entity component : inventory.contents)
            {
                auto component_tags = registry.get<component::Tags>(component);
                tags                = ETag(tags | component_tags.tags);
            }
                inventory.tags      = tags;
        }
    });
}
} // namespace cs::system
