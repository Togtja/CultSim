#include "location_requirement.h"
#include "ai/path_finding.h"
#include "common_helpers.h"
#include "entity/components/components.h"

#include <spdlog/spdlog.h>
namespace cs::action
{
LocationRequirement::~LocationRequirement()
{
    m_dispatcher.sink<event::ArrivedAtDestination>().disconnect<&LocationRequirement::event_listener>(this);
};

void LocationRequirement::init(entt::entity entity)
{
    owner     = entity;
    auto& mov = m_registry.get<component::Movement>(owner);
    auto& pos = m_registry.get<component::Position>(owner);
    if (!path_finding(pos.position, m_desired_pos, mov.desired_position))
    {
        spdlog::warn("failed to find path");
    }
    m_dispatcher.sink<event::ArrivedAtDestination>().connect<&LocationRequirement::event_listener>(this);
}
IRequirement* LocationRequirement::clone()
{
    auto copy = new LocationRequirement(name, m_registry, m_desired_pos, m_dispatcher);
    return copy;
}
void LocationRequirement::event_listener(const event::ArrivedAtDestination& event)
{
    if (event.entity == owner && cs::close_enough(event.position, m_desired_pos, 10.f))
    {
        predicate = true;
    }
}
} // namespace cs::action
