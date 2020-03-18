#include "lua_type_bindings.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/strategy.h"
#include "entity/components/tags.h"
#include "entity/scenario.h"
#include "entity/systems/system.h"
#include "input/input_handler.h"
#include "random_engine.h"

#include <entt/entity/registry.hpp>
#include <entt/entity/runtime_view.hpp>

namespace cs::lua
{
void bind_dataonly(sol::state_view lua)
{
    /** Vectors */
    lua.new_usertype<glm::vec2>("Vec2",
                                sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
                                "x",
                                &glm::vec2::x,
                                "y",
                                &glm::vec2::y);

    lua.new_usertype<glm::vec3>("Vec3",
                                sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
                                "x",
                                &glm::vec3::x,
                                "y",
                                &glm::vec3::y,
                                "z",
                                &glm::vec3::z);

    lua.new_enum<ETag>("ETag",
                       {{"None", ETag::TAG_None},
                        {"Food", ETag::TAG_Food},
                        {"Drink", ETag::TAG_Drink},
                        {"Sleep", ETag::TAG_Sleep},
                        {"Joy", ETag::TAG_Joy},
                        {"Location", ETag::TAG_Location},
                        {"Find", ETag::TAG_Find},
                        {"Vision", ETag::TAG_Vision},
                        {"Avoidable", ETag::TAG_Avoidable},
                        {"Reproduce", ETag::TAG_Reproduce},
                        {"Human", ETag::TAG_Human},
                        {"Tag", ETag::TAG_Tag},
                        {"Gather", ETag::TAG_Gather}});
}

void bind_components(sol::state_view lua)
{
    /** Component Helpers */
    lua.new_usertype<ai::Need>("Need",
                               "name",
                               &ai::Need::name,
                               "weight",
                               &ai::Need::weight,
                               "status",
                               &ai::Need::status,
                               "decay_rate",
                               &ai::Need::decay_rate,
                               "vitality",
                               &ai::Need::vitality,
                               "tags",
                               &ai::Need::tags);

    lua.new_usertype<action::Action>("Action",
                                     "name",
                                     &action::Action::name,
                                     "requirements",
                                     &action::Action::requirements,
                                     "time_to_complete",
                                     &action::Action::time_to_complete,
                                     "success_chance",
                                     &action::Action::success_chance,
                                     "success",
                                     &action::Action::success,
                                     "failure",
                                     &action::Action::failure,
                                     "abort",
                                     &action::Action::abort);

    lua.new_enum<component::Reproduction::ESex>(
        "ESex",
        {{"Male", component::Reproduction::ESex::Male}, {"Female", component::Reproduction::ESex::Female}});

    /** Components */
    lua.new_usertype<component::Position>("PositionComponent", "position", &component::Position::position);
    lua.new_usertype<component::Sprite>("SpriteComponent", "color", &component::Sprite::color); // ignoring texture for now

    /**
     * Ignoring avoidance_cd and avoid_count since those can be changed using the UI and ignoring desired_position as that is
     * a part of the pathfinding algorithm
     */
    lua.new_usertype<component::Movement>("MovementComponent",
                                          "direction",
                                          &component::Movement::direction,
                                          "speed",
                                          &component::Movement::speed);

    lua.new_usertype<component::Vision>("VisionComponent", "radius", &component::Vision::radius, "fov", &component::Vision::fov);

    lua.new_usertype<component::Hearing>("HearingComponent", "radius", &component::Hearing::radius);
    lua.new_usertype<component::Smell>("SmellComponent", "radius", &component::Smell::radius);

    lua.new_usertype<component::Reproduction>("ReproductionComponent",
                                              "sex",
                                              &component::Reproduction::sex,
                                              "max_children",
                                              &component::Reproduction::max_children);

    lua.new_usertype<component::Health>("HealthComponet",
                                        "health",
                                        &component::Health::health,
                                        "tickdown_rate",
                                        &component::Health::tickdown_rate);

    lua.new_usertype<component::Tags>("TagComponent", "tags", &component::Tags::tags);

    lua.new_usertype<component::Need>("NeedComponent", "needs", &component::Need::needs);

    lua.new_usertype<component::Strategy>("StrategyComponent", "strategies", &component::Strategy::strategies);

    /** Entity registry, we only expose a limited number of functions here */
    lua.new_usertype<entt::registry>("Registry", "valid", &entt::registry::valid);
}

void bind_systems(sol::state_view lua)
{
    lua.new_usertype<system::ISystem>("ISystem", "update", &system::ISystem::update);

    lua.new_usertype<Scenario>("Scenario",
                               "name",
                               &Scenario::name,
                               "description",
                               &Scenario::description,
                               "systems",
                               &Scenario::systems,
                               "bounds",
                               &Scenario::bounds,
                               "sampling_rate",
                               &Scenario::sampling_rate,
                               "init",
                               &Scenario::init,
                               "update",
                               &Scenario::update,
                               "draw",
                               &Scenario::draw,
                               "shutdown",
                               &Scenario::shutdown);
}

void bind_input(sol::state_view lua)
{
    lua.new_enum<input::EKeyContext>("EKeyContext",
                                     {{"DefaultContext", input::EKeyContext::DefaultContext},
                                      {"Agent", input::EKeyContext::Agent},
                                      {"AgentOnHover", input::EKeyContext::AgentOnHover},
                                      {"ScenarioScene", input::EKeyContext::ScenarioScene}});

    lua.new_enum<input::EAction>("EAction",
                                 {{"MoveUp", input::EAction::MoveUp},
                                  {"MoveDown", input::EAction::MoveDown},
                                  {"MoveLeft", input::EAction::MoveLeft},
                                  {"MoveRight", input::EAction::MoveRight},
                                  {"PauseMenu", input::EAction::PauseMenu},
                                  {"ZoomIn", input::EAction::ZoomIn},
                                  {"ZoomOut", input::EAction::ZoomOut},
                                  {"SelectEntity", input::EAction::SelectEntity},
                                  {"FollowEntity", input::EAction::FollowEntity},
                                  {"SpeedUp", input::EAction::SpeedUp},
                                  {"SpeedDown", input::EAction::SpeedDown},
                                  {"Pause", input::EAction::Pause}});

    lua.new_enum<input::EMouse>("EMouse",
                                {{"BtnLeft", input::EMouse::BtnLeft},
                                 {"BtnRight", input::EMouse::BtnRight},
                                 {"BtnMiddle", input::EMouse::BtnMiddle},
                                 {"BtnMove", input::EMouse::BtnMove},
                                 {"BtnWheelUp", input::EMouse::BtnWheelUp},
                                 {"BtnWheelDown", input::EMouse::BtnWheelDown},
                                 {"BtnWheelLeft", input::EMouse::BtnWheelLeft},
                                 {"BtnWheelRight", input::EMouse::BtnWheelRight},
                                 {"BtnX1", input::EMouse::BtnX1},
                                 {"BtnX2", input::EMouse::BtnX2}});
}

void bind_utils(sol::state_view lua)
{
    /** Enable use of random number generator */
    lua.new_usertype<RandomEngine>("RandomEngine",
                                   sol::no_constructor,
                                   "uniform",
                                   &RandomEngine::uniform<float>,
                                   "randint",
                                   &RandomEngine::uniform<int>,
                                   "normal",
                                   &RandomEngine::normal,
                                   "trigger",
                                   &RandomEngine::trigger,
                                   "roll",
                                   &RandomEngine::roll);
}

} // namespace cs::lua
