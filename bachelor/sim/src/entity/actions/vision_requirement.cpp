#include "vision_requirement.h"
#include "entity/components/components.h"
#include "common_helpers.h"

namespace cs::action
{
VisionRequirement::~VisionRequirement()
{
    m_dispatcher.sink<event::SensedEntity>().disconnect<&VisionRequirement::event_listener>(this);
}
void VisionRequirement::init(entt::entity entity)
{
    owner = entity;
    m_dispatcher.sink<event::SensedEntity>().connect<&VisionRequirement::event_listener>(this);
}
IRequirement* VisionRequirement::clone()
{
    auto copy = new VisionRequirement(name, m_registry, m_tags, m_dispatcher);
    return copy;
}
void VisionRequirement::event_listener(const event::SensedEntity& event)
{
    auto tags = m_registry.get<component::Tags>(event.entity);
    if (count_set_bits(tags.tags&m_tags)) 
    {
        predicate = true;
    }
}
} // namespace cs::action