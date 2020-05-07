#pragma once

#include "data_collector.h"

#include <entt/fwd.hpp>
#include <sol/table.hpp>

namespace cs::debug
{
/**
 * Checks for number of entities with a HealthComponent and assumes that to be living entities
 */
class CollectorLivingEntities : public DataCollector::Command
{
private:
    entt::registry* m_registry{nullptr};

public:
    CollectorLivingEntities(entt::registry& registry);

    float execute() override;
    std::string_view get_name() override;
};

/**
 * Collects the average health of all entities through the HealthComponent
 */
class CollectorAverageHealth : public DataCollector::Command
{
private:
    entt::registry* m_registry = nullptr;

public:
    CollectorAverageHealth(entt::registry& registry);

    float execute() override;
    std::string_view get_name() override;
};

/**
 * Data collector that fetches the average value of a need
 */
class CollectorNeed : public DataCollector::Command
{
private:
    entt::registry* m_registry{nullptr};

    /** Need to collect */
    std::string m_need_name{};

public:
    CollectorNeed(entt::registry& registry, const std::string& need_name);

    float execute() override;
    std::string_view get_name() override;
};

/**
 * Data collector that fetches normalized mouse coordinates
 */
class CollectorMouse : public DataCollector::Command
{
private:
    bool m_x_axis{false};

    glm::ivec2 m_resolution{};

    std::string m_name{};

public:
    CollectorMouse(bool x_axis, glm::ivec2 resolution);

    float execute() override;
    std::string_view get_name() override;
};

/**
 * Base class for creating custom data collectors in Lua
 */
class LuaCollector : public DataCollector::Command
{
private:
    std::string m_name{};

    sol::table m_data{};

public:
    LuaCollector(std::string name, sol::table data) : m_name(name), m_data(data)
    {
    }

    float execute() override
    {
        return m_data["execute"]();
    }

    std::string_view get_name() override
    {
        return m_name;
    }
};
} // namespace cs::debug
