#include "native_collectors.h"
#include "entity/components/components.h"

#include <entt/entity/registry.hpp>

namespace cs::debug
{
CollectorLivingEntities::CollectorLivingEntities(entt::registry& registry) : m_registry(&registry)
{
}

float cs::debug::CollectorLivingEntities::execute()
{
    return m_registry->size<component::Health>();
}

std::string_view cs::debug::CollectorLivingEntities::get_name()
{
    return "Living Entities";
}

CollectorAverageHealth::CollectorAverageHealth(entt::registry& registry) : m_registry(&registry)
{
}

float CollectorAverageHealth::execute()
{
    float avg        = 0.f;
    auto health_view = m_registry->view<component::Health>();
    health_view.each([&avg](const component::Health& health) { avg += health.health; });
    return avg / health_view.size();
}

std::string_view CollectorAverageHealth::get_name()
{
    return "Average Health";
}

} // namespace cs::debug
