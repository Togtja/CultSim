#include "native_collectors.h"
#include "entity/components/components.h"
#include "input/input_handler.h"

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
    if (health_view.empty())
    {
        return avg;
    }

    return avg / health_view.size();
}

std::string_view CollectorAverageHealth::get_name()
{
    return "Average Health";
}

CollectorNeed::CollectorNeed(entt::registry& registry, const std::string& need_name) :
    m_registry(&registry),
    m_need_name(need_name)
{
}

float CollectorNeed::execute()
{
    float avg      = 0.f;
    auto need_view = m_registry->view<component::Need>();

    need_view.each([&avg, this](const component::Need& need) {
        avg += std::find_if(need.needs.cbegin(), need.needs.cend(), [this](const ai::Need& n) {
                   return n.name == m_need_name;
               })->status;
    });

    if (need_view.empty())
    {
        return avg;
    }

    return avg / need_view.size();
}

std::string_view CollectorNeed::get_name()
{
    return m_need_name;
}

CollectorMouse::CollectorMouse(bool x_axis, glm::ivec2 resolution) : m_x_axis(x_axis), m_resolution(resolution)
{
    if (x_axis)
    {
        m_name = "Mouse X";
    }
    else
    {
        m_name = "Mouse Y";
    }
}

float CollectorMouse::execute()
{
    if (m_x_axis)
    {
        return input::get_input().get_mouse_pos().x / static_cast<float>(m_resolution.x);
    }
    else
    {
        return input::get_input().get_mouse_pos().y / static_cast<float>(m_resolution.y);
    }
}

std::string_view CollectorMouse::get_name()
{
    return m_name;
}

} // namespace cs::debug
