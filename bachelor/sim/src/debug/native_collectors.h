#pragma once

#include "data_collector.h"

#include <entt/fwd.hpp>

namespace cs::debug
{
class CollectorLivingEntities : DataCollector::Command
{
private:
    /** Registry */
    entt::registry* m_registry = nullptr;

public:
    CollectorLivingEntities(entt::registry& registry);

    float execute() override;
    std::string_view get_name() override;
};
} // namespace cs::debug
