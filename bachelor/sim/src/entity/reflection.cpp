#include "reflection.h"
#include "entity/components/components.h"
#include "entity/systems/action.h"
#include "entity/systems/deletion.h"
#include "entity/systems/goal.h"
#include "entity/systems/gob.h"
#include "entity/systems/gobbas.h"
#include "entity/systems/health.h"
#include "entity/systems/inventory.h"
#include "entity/systems/memory.h"
#include "entity/systems/mitigation.h"
#include "entity/systems/movement.h"
#include "entity/systems/need.h"
#include "entity/systems/relationship.h"
#include "entity/systems/rendering.h"
#include "entity/systems/reproduction.h"
#include "entity/systems/requirement.h"
#include "entity/systems/sensor.h"
#include "entity/systems/timer.h"
#include "entity/systems/traits.h"

#include <entt/core/hashed_string.hpp>
#include <entt/meta/factory.hpp>

using entt::hashed_string;

namespace cs::meta
{
void reflect_data_types()
{
    /** GLM Vectors */
    entt::meta<glm::vec2>()
        .alias("vec2"_hs)
        .data<&glm::vec2::x>("x"_hs)
        .data<&glm::vec2::y>("y"_hs)
        .ctor<float>()
        .ctor<float, float>();

    entt::meta<glm::vec3>()
        .alias("vec3"_hs)
        .data<&glm::vec3::x>("x"_hs)
        .data<&glm::vec3::y>("y"_hs)
        .data<&glm::vec3::z>("z"_hs)
        .ctor<float>()
        .ctor<float, float, float>();

    /** Component Helpers */
    entt::meta<ai::Need>()
        .alias("Need"_hs)
        .data<&ai::Need::name>("name"_hs)
        .data<&ai::Need::status>("status"_hs)
        .data<&ai::Need::weight>("weight"_hs)
        .data<&ai::Need::decay_rate>("decay_rate"_hs)
        .data<&ai::Need::tags>("tags"_hs);

    /** Core Components */
    entt::meta<component::Position>().alias("PositionComponent"_hs).data<&component::Position::position>("position"_hs);

    /** Ignoring "seen" on vision as it's only going to be populated by sensor systems */
    entt::meta<component::Vision>()
        .alias("VisionComponent"_hs)
        .data<&component::Vision::radius>("vision_radius"_hs)
        .data<&component::Vision::fov>("fov"_hs);

    entt::meta<component::Need>()
        .alias("NeedComponent"_hs)
        .data<&component::Need::needs>("needs"_hs)
        .data<&component::Need::vital_needs>("pressing_needs"_hs);

    /** Keep adding the components we've used so far (but only add members that make sense to set defaults for in Lua)... */
}

void reflect_systems()
{
    entt::meta<system::ISystem>()
        .alias("ISystem"_hs)
        .func<&system::ISystem::update>("update"_hs)
        .func<&system::ISystem::initialize>("initialize"_hs)
        .func<&system::ISystem::deinitialize>("deinitialize"_hs);
    entt::meta<system::Action>().alias("ActionSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Sensor>().alias("SensorSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Health>().alias("HealthSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Mitigation>().alias("MitigationSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Movement>().alias("MovementSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Need>().alias("NeedSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Reproduction>().alias("ReproductionSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Requirement>().alias("RequirementSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Timer>().alias("TimerSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Memory>().alias("MemorySystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Deletion>().alias("DeletionSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Inventory>().alias("InventorySystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Goal>().alias("GoalSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::GOB>().alias("GOBSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::GOBBAS>().alias("GOBBASystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Traits>().alias("TraitsSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();
    entt::meta<system::Relationship>().alias("RelationshipSystem"_hs).ctor<system::SystemContext>().base<system::ISystem>();

}

} // namespace cs::meta
