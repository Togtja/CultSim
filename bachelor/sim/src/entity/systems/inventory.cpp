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
    auto group     = registry.group<component::Tags, component::Inventory>();

    group.each([this, dt, &registry](entt::entity e, component::Inventory& inventory) {
        if (inventory.size != inventory.contents.size())
        {
            inventory.size = inventory.contents.size();
            for (entt::entity component : inventory.contents)
            {
                auto component_tags = registry.get<component::Tags>(component);
                inventory.tags      = ETag(inventory.tags | component_tags.tags);
            }
        }
    });
}
} // namespace cs::system
