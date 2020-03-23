#include "inventory.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include <spdlog/spdlog.h>

namespace cs::system
{
void Inventory::initialize()
{
    m_context.dispatcher->sink<event::DeleteEntity>().connect<&Inventory::drop_items>(*this);
}

void Inventory::deinitialize()
{
    m_context.dispatcher->sink<event::DeleteEntity>().disconnect<&Inventory::drop_items>(*this);
}

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
    });
}

ISystem* Inventory::clone()
{
    return new Inventory(m_context);
}

void Inventory::drop_items(const event::DeleteEntity& event)
{
    if (auto inventory = m_context.registry->try_get<component::Inventory>(event.entity); inventory)
    {
        if (auto pos = m_context.registry->try_get<component::Position>(event.entity); pos)
        {
            for (entt::entity component : inventory->contents)
            {
                if (!m_context.registry->has<component::Position>(component))
                {
                    m_context.registry->assign<component::Position>(component,
                                                                    glm::vec3{pos->position.x + m_context.rng->uniform(0.f, 10.f),
                                                                              pos->position.y + m_context.rng->uniform(0.f, 10.f),
                                                                              pos->position.z});
                }
            }
        }
    }
}
} // namespace cs::system
