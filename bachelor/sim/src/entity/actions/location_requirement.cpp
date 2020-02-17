#include "location_requirement.h"
#include "common_helpers.h"
#include "entity/components/components.h"
namespace cs::action
{
void LocationRequirement::init()
{
    auto pos             = m_registry.get<component::Position>(owner);
    pos.desired_position = m_desired_pos;
    m_dispatcher.sink<event::ArrivedAtDestination>().connect<&LocationRequirement::event_listener>(this);
}
void LocationRequirement::event_listener(event::ArrivedAtDestination& event)
{
    if (event.entity == owner && cs::close_enough(event.position, m_desired_pos, 2.f))
    {
        predicate = true;
    }
}
} // namespace cs::action
