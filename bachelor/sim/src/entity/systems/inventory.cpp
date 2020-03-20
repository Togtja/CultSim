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
    auto view      = registry.view<component::Inventory>();

    view.each([this, dt, &registry](entt::entity e, component::Inventory& inventory) {
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

        if (auto pos = registry.try_get<component::Position>(e); pos && registry.try_get<component::Delete>(e))
        {
            for (entt::entity component : inventory.contents)
            {
                if (!registry.has<component::Position>(component))
                {
                    registry.assign<component::Position>(component,
                                                         glm::vec3{pos->position.x + m_context.rng->uniform(0.f, 10.f),
                                                                   pos->position.y + m_context.rng->uniform(0.f, 10.f),
                                                                   pos->position.z});
                }
            }
        }
    });
}
} // namespace cs::system
