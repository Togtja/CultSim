#pragma once

#include "data_collector.h"

#include <entt/fwd.hpp>

namespace cs::debug
{
class CollectorLivingEntities : public DataCollector::Command
{
private:
    /** Registry */
    entt::registry* m_registry = nullptr;

public:
    CollectorLivingEntities(entt::registry& registry);

    float execute() override;
    std::string_view get_name() override;
};

class CollectorAverageHealth : public DataCollector::Command
{
private:
    /** Registry */
    entt::registry* m_registry = nullptr;

public:
    CollectorAverageHealth(entt::registry& registry);

    float execute() override;
    std::string_view get_name() override;
};

class CollectorNeed : public DataCollector::Command
{
private:
    /** Registry */
    entt::registry* m_registry = nullptr;

    /** Need to collect */
    std::string m_need_name{};

public:
    CollectorNeed(entt::registry& registry, const std::string& need_name);

    float execute() override;
    std::string_view get_name() override;
};
} // namespace cs::debug
