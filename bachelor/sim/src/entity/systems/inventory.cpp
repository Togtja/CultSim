#include "inventory.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include <spdlog/spdlog.h>

namespace cs::system
{
void Inventory::update(float dt)
{
    CS_AUTOTIMER(Inventory System);

    auto& registry = *m_context.registry;
    auto view      = registry.view<component::Inventory, component::Tags, component::Position>();

    view.each([this, dt, &registry](entt::entity e,
                                    component::Inventory& inventory,
                                    const component::Tags& tags,
                                    const component::Position& pos) {
        if (inventory.size != inventory.contents.size())
        {
            inventory.size = inventory.contents.size();
            ETag tags{};
            for (entt::entity component : inventory.contents)
            {
                if (auto component_tags = registry.try_get<component::Tags>(component); component_tags)
                {
                    tags = ETag(tags | component_tags->tags);
                }
                else
                {
                    spdlog::get("agent")->debug("entity {} did not have any tags", component);
                }
            }
            inventory.tags = tags;
        }
        if (tags.tags & TAG_Delete)
        {
            for (entt::entity component : inventory.contents)
            {
                glm::vec3 new_pos{pos.position.x + m_context.rng->uniform(-10.f, 10.f),
                                  pos.position.y + m_context.rng->uniform(-10.f, 10.f),
                                  pos.position.z};
                m_context.registry->assign_or_replace<component::Position>(component, new_pos);
            }
            inventory.contents.clear();
        }
    });
}

ISystem* Inventory::clone()
{
    return new Inventory(m_context);
}

} // namespace cs::system
