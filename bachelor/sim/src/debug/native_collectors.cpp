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

} // namespace cs::debug
