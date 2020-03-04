#include "lua_type_bindings.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/strategy.h"
#include "entity/components/tags.h"
#include "entity/scenario.h"
#include "entity/systems/system.h"
#include "input/input_handler.h"
#include "random_engine.h"

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
                       {{"Food", ETag::TAG_Food},
                        {"Drink", ETag::TAG_Drink},
                        {"Sleep", ETag::TAG_Sleep},
                        {"Joy", ETag::TAG_Joy},
                        {"Location", ETag::TAG_Location},
                        {"Find", ETag::TAG_Find},
                        {"Vision", ETag::TAG_Vision},
                        {"Avoidable", ETag::TAG_Avoidable}});
    lua.new_enum<component::Reproduction::ESex>(
        "ESex",
        {{"Male", component::Reproduction::ESex::Male}, {"Female", component::Reproduction::Female}});
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
                               "tags",
                               &ai::Need::tags);

    /** Components */
    lua.new_usertype<component::Position>("PositionComponent", "position", &component::Position::position);
    lua.new_usertype<component::Sprite>("SpriteComponent", "color", &component::Sprite::color); // ignoring texture for now
    // Ignoring Avoidance_cs and Avoid_count since those cna be change using the UI and ignoring desired_position as that is
    // a part of the pathfinding algorithm
    lua.new_usertype<component::Movement>("MovementComponent",
                                          "direction",
                                          &component::Movement::direction,
                                          "speed",
                                          &component::Movement::speed);

    lua.new_usertype<component::Vision>("VisionComponent",
                                        "vision radius",
                                        &component::Vision::vision_radius,
                                        "field of view",
                                        &component::Vision::fov);

    lua.new_usertype<component::Hearing>("HearingComponent", "hearing radius", &component::Hearing::sound_radius);
    lua.new_usertype<component::Smell>("SmellComponent", "smell radius", &component::Smell::smell_radius);

    lua.new_usertype<component::Reproduction>("ReproductionComponent",
                                              "sex",
                                              &component::Reproduction::sex,
                                              "max children",
                                              &component::Reproduction::number_of_children);

    lua.new_usertype<component::Health>("HealthComponet",
                                        "health",
                                        &component::Health::hp,
                                        "tickdown rate",
                                        &component::Health::tickdown_rate);
}

void bind_systems(sol::state_view lua)
{
    lua.new_usertype<system::ISystem>("ISystem", "update", &system::ISystem::update);

    lua.new_usertype<Scenario>("Scenario",
                               "name",
                               &Scenario::name,
                               "description",
                               &Scenario::description,
                               "agent_count",
                               &Scenario::agent_count,
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
    lua.new_enum<input::KeyContext>("EKeyContext",
                                    {{"DefaultContext", input::KeyContext::DefaultContext},
                                     {"Agent", input::KeyContext::Agent},
                                     {"AgentOnHover", input::KeyContext::AgentOnHover},
                                     {"ScenarioScene", input::KeyContext::ScenarioScene}});

    lua.new_enum<input::Action>("EAction",
                                {{"MoveUp", input::Action::MoveUp},
                                 {"MoveDown", input::Action::MoveDown},
                                 {"MoveLeft", input::Action::MoveLeft},
                                 {"MoveRight", input::Action::MoveRight},
                                 {"Pause", input::Action::Pause},
                                 {"ZoomIn", input::Action::ZoomIn},
                                 {"ZoomOut", input::Action::ZoomOut},
                                 {"SelectEntity", input::Action::SelectEntity},
                                 {"FollowEntity", input::Action::FollowEntity}});

    lua.new_enum<input::Mouse>("EMouse",
                               {{"Left", input::Mouse::Left},
                                {"Right", input::Mouse::Right},
                                {"Middle", input::Mouse::Middle},
                                {"Move", input::Mouse::Move},
                                {"WheelUp", input::Mouse::WheelUp},
                                {"WheelDown", input::Mouse::WheelDown},
                                {"WheelLeft", input::Mouse::WheelLeft},
                                {"WheelRight", input::Mouse::WheelRight},
                                {"X1", input::Mouse::X1},
                                {"X2", input::Mouse::X2}});
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
