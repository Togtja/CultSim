#include "reflection.h"
#include "entity/components/components.h"
#include "entity/systems/action.h"
#include "entity/systems/ai.h"
#include "entity/systems/mitigation.h"
#include "entity/systems/movement.h"
#include "entity/systems/need.h"
#include "entity/systems/rendering.h"
#include "entity/systems/requirement.h"
#include "entity/systems/timer.h"

#include <entt/core/hashed_string.hpp>
#include <entt/meta/factory.hpp>

using entt::hashed_string;

namespace cs::meta
{
void reflect_data_types()
{
    /** GLM Vectors */
    entt::meta<glm::vec2>()
        .type("vec2"_hs)
        .data<&glm::vec2::x>("x"_hs)
        .data<&glm::vec2::y>("y"_hs)
        .ctor<float>()
        .ctor<float, float>();

    entt::meta<glm::vec3>()
        .type("vec3"_hs)
        .data<&glm::vec3::x>("x"_hs)
        .data<&glm::vec3::y>("y"_hs)
        .data<&glm::vec3::z>("z"_hs)
        .ctor<float>()
        .ctor<float, float, float>();

    /** Component Helpers */
    entt::meta<ai::Need>()
        .type("Need"_hs)
        .data<&ai::Need::name>("name"_hs)
        .data<&ai::Need::status>("status"_hs)
        .data<&ai::Need::weight>("weight"_hs)
        .data<&ai::Need::decay_rate>("decay_rate"_hs)
        .data<&ai::Need::tags>("tags"_hs);

    /** Core Components */
    entt::meta<component::Position>().type("PositionComponent"_hs).data<&component::Position::position>("position"_hs);

    /** Ignoring "seen" on vision as it's only going to be populated by sensor systems */
    entt::meta<component::Vision>()
        .type("VisionComponent"_hs)
        .data<&component::Vision::vision_radius>("vision_radius"_hs)
        .data<&component::Vision::fov>("fov"_hs);

    entt::meta<component::Needs>()
        .type("NeedComponent"_hs)
        .data<&component::Needs::needs>("needs"_hs)
        .data<&component::Needs::pressing_needs>("pressing_needs"_hs);

    /** Keep adding the components we've used so far... */
}

void reflect_systems()
{
    entt::meta<system::ISystem>().type("ISystem"_hs).func<&system::ISystem::update>("update"_hs);
    entt::meta<system::Action>().type("ActionSystem"_hs).base<system::ISystem>();
    entt::meta<system::AI>().type("AiSystem"_hs).base<system::ISystem>();
    entt::meta<system::Mitigation>().type("MitigationSystem"_hs).base<system::ISystem>();
    entt::meta<system::Movement>().type("MovementSystem"_hs).base<system::ISystem>();
    entt::meta<system::Need>().type("NeedSystem"_hs).base<system::ISystem>();
    entt::meta<system::Rendering>().type("RenderingSystem"_hs).base<system::ISystem>();
    entt::meta<system::Requirement>().type("RequirementSystem"_hs).base<system::ISystem>();
}

} // namespace cs::meta
