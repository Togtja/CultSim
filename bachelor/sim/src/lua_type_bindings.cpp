#include "lua_type_bindings.h"
#include "debug/native_collectors.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/strategy.h"
#include "entity/components/tags.h"
#include "entity/name_generator.h"
#include "entity/scenario.h"
#include "entity/systems/lua_system.h"
#include "input/input_handler.h"
#include "preferences.h"
#include "random_engine.h"

#include <entt/entity/registry.hpp>
#include <entt/entity/runtime_view.hpp>
#include <spdlog/spdlog.h>

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
                        {"Creature", ETag::TAG_Creature},
                        {"Tag", ETag::TAG_Tag},
                        {"Gather", ETag::TAG_Gather},
                        {"Reserved", ETag::TAG_Reserved},
                        {"Delete", ETag::TAG_Delete},
                        {"Carnivore", ETag::TAG_Carnivore},
                        {"Herbivore", ETag::TAG_Herbivore},
                        {"Omnivore", ETag::TAG_Omnivore},
                        {"Meat", ETag::TAG_Meat},
                        {"Veggie", ETag::TAG_Veggie},
                        {"Inventory", ETag::TAG_Inventory},
                        {"Consume", ETag::TAG_Consume},
                        {"Crime", ETag::TAG_Crime},
                        {"Hostile", ETag::TAG_Hostile}});

    lua.new_usertype<NameGenerator::Name>("GeneratedName",
                                          "first",
                                          &NameGenerator::Name::first,
                                          "last",
                                          &NameGenerator::Name::last);

    lua.new_usertype<system::BothParentName>("Parents", "mom", &system::BothParentName::mom, "dad", &system::BothParentName::dad);

    lua.new_usertype<system::ParentName>("Parent", "name", &system::ParentName::name, "ids", &system::ParentName::ids);

    lua.new_usertype<system::ParentEntityIds>("ParentIds",
                                              "global",
                                              &system::ParentEntityIds::global,
                                              "relationship",
                                              &system::ParentEntityIds::relationship);
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
                               &ai::Need::tags,
                               "weight_multi",
                               &ai::Need::weight_multi,
                               "decay_multi",
                               &ai::Need::decay_multi,
                               "weight_func",
                               &ai::Need::weight_func);

    lua.new_usertype<gob::Goal>("Goal",
                                "name",
                                &gob::Goal::m_name,
                                "tags",
                                &gob::Goal::m_tags,
                                "age",
                                &gob::Goal::m_age,
                                "weight_function",
                                &gob::Goal::m_weight_function,
                                "change_over_time",
                                &gob::Goal::m_change_over_time,
                                "get_discontentment",
                                &gob::Goal::m_get_discontentment);

    lua.new_usertype<gob::ActionSequence>("ActionSequence",
                                          "name",
                                          &gob::ActionSequence::m_name,
                                          "tags",
                                          &gob::ActionSequence::m_tags,
                                          "current_action",
                                          &gob::ActionSequence::current_action,
                                          "actions",
                                          &gob::ActionSequence::m_actions,
                                          "run_actions",
                                          &gob::ActionSequence::m_run_actions,
                                          "get_duration",
                                          &gob::ActionSequence::m_get_duration,
                                          "get_goal_change",
                                          &gob::ActionSequence::m_get_goal_change);

    lua.new_usertype<gob::Action>("GOB_Action",
                                  "name",
                                  &gob::Action::name,
                                  "tags",
                                  &gob::Action::tags,
                                  "required_time",
                                  &gob::Action::m_required_time,
                                  "success_chance",
                                  &gob::Action::m_success_chance,
                                  "flags",
                                  &gob::Action::m_flags,
                                  "action",
                                  &gob::Action::m_action,
                                  "get_goal_change",
                                  &gob::Action::m_get_goal_change,
                                  "get_duration",
                                  &gob::Action::m_get_duration);

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

    lua.new_usertype<component::Inventory>("InventoryComponent",
                                           "max_size",
                                           &component::Inventory::max_size,
                                           "size",
                                           &component::Inventory::size,
                                           "tags",
                                           &component::Inventory::tags,
                                           "contents",
                                           &component::Inventory::contents);

    /**
     * Ignoring avoidance_cd and avoid_count since those can be changed using the UI and ignoring desired_position as that is
     * a part of the pathfinding algorithm
     */
    lua.new_usertype<component::Movement>("MovementComponent",
                                          "direction",
                                          &component::Movement::direction,
                                          "speed",
                                          &component::Movement::speed,
                                          "speed_multi",
                                          &component::Movement::speed_multi,
                                          "desired_position",
                                          &component::Movement::desired_position);

    lua.new_usertype<component::Vision>("VisionComponent",
                                        "seen",
                                        &component::Vision::seen,
                                        "radius",
                                        &component::Vision::radius,
                                        "fov",
                                        &component::Vision::fov);

    lua.new_usertype<component::Hearing>("HearingComponent", "radius", &component::Hearing::radius);
    lua.new_usertype<component::Smell>("SmellComponent", "radius", &component::Smell::radius);

    lua.new_usertype<component::Reproduction>("ReproductionComponent", "sex", &component::Reproduction::sex);

    lua.new_usertype<component::Health>("HealthComponet",
                                        "health",
                                        &component::Health::health,
                                        "tickdown_rate",
                                        &component::Health::tickdown_rate);

    lua.new_usertype<component::Tags>("TagComponent", "tags", &component::Tags::tags);

    lua.new_usertype<component::Need>("NeedComponent",
                                      "required_needs",
                                      &component::Need::needs,
                                      "leisure_needs",
                                      &component::Need::leisure_needs);

    lua.new_usertype<component::Strategy>("StrategyComponent", "strategies", &component::Strategy::strategies);

    lua.new_usertype<component::Attack>("AttackComponent", "damage", &component::Attack::damage);

    lua.new_usertype<component::Age>("AgeComponent", "life_expectancy", &component::Age::average_life_expectancy);

    lua.new_usertype<component::Name>("NameComponent",
                                      "entity_type",
                                      &component::Name::entity_type,
                                      "name",
                                      &component::Name::name);

    lua.new_usertype<component::Action>("ActionComponent", "actions", &component::Action::actions);

    lua.new_usertype<component::Goal>("GoalComponent", "goals", &component::Goal::goals);

    lua.new_usertype<component::detail::Trait>("Trait",
                                               "name",
                                               &component::detail::Trait::name,
                                               "desc",
                                               &component::detail::Trait::desc,
                                               "can_inherit",
                                               &component::detail::Trait::can_inherit,
                                               "inherit_chance",
                                               &component::detail::Trait::inherit_chance,
                                               "can_mutate",
                                               &component::detail::Trait::can_mutate,
                                               "mutation_chance",
                                               &component::detail::Trait::mutation_chance,
                                               "attain",
                                               &component::detail::Trait::attain,
                                               "lose",
                                               &component::detail::Trait::lose,
                                               "affect",
                                               &component::detail::Trait::affect,
                                               "unaffect",
                                               &component::detail::Trait::unaffect);

    /** Entity registry, we only expose a limited number of functions here */
    lua.new_usertype<entt::registry>("Registry", "valid", &entt::registry::valid);

    /** Bind View */
    lua.new_usertype<entt::runtime_view>("View",
                                         sol::no_constructor,
                                         "empty",
                                         &entt::runtime_view::empty,
                                         "size",
                                         &entt::runtime_view::size,
                                         "each",
                                         &entt::runtime_view::each<sol::function>,
                                         "contains",
                                         &entt::runtime_view::contains);
}

void bind_systems(sol::state_view lua)
{
    lua.new_usertype<system::LuaSystem>("System",
                                        sol::no_constructor,
                                        "initialize",
                                        &system::LuaSystem::initialize,
                                        "deinitialize",
                                        &system::LuaSystem::deinitialize,
                                        "update",
                                        &system::LuaSystem::update);

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
                                      {"ScenarioScene", input::EKeyContext::ScenarioScene},
                                      {"PauseMenu", input::EKeyContext::PauseMenu},
                                      {"PreferenceScene", input::EKeyContext::PreferenceScene},
                                      {"EditorScene", input::EKeyContext::EditorScene},
                                      {"RenderingSystem", input::EKeyContext::RenderingSystem},
                                      {"LoadScenario", input::EKeyContext::LoadScenario}});

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
                                  {"Pause", input::EAction::Pause},
                                  {"ReloadShaders", input::EAction::ReloadShaders},
                                  {"Quit", input::EAction::Quit},
                                  {"SetMode2D", input::EAction::SetMode2D},
                                  {"SetMode3D", input::EAction::SetMode3D},
                                  {"EscapeScene", input::EAction::EscapeScene}});

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

    lua.new_usertype<Preference>("Preference",
                                 sol::no_constructor,
                                 "name",
                                 sol::readonly(&Preference::name),
                                 "description",
                                 sol::readonly(&Preference::description),
                                 "value",
                                 &Preference::value);

    lua.new_usertype<PreferenceManager>("PreferenceManager",
                                        sol::no_constructor,
                                        "get_resolution",
                                        &PreferenceManager::get_resolution,
                                        "set_resolution",
                                        &PreferenceManager::set_resolution,
                                        "get_fullscreen",
                                        &PreferenceManager::get_fullscreen,
                                        "set_fullscreen",
                                        &PreferenceManager::set_fullscreen);

    /** Lua users need to create one of these and fill them with a table that contains at least an execute function returning
     * float. */
    lua.new_usertype<debug::LuaCollector>("DataCollector", sol::constructors<debug::LuaCollector(std::string, sol::table)>());
}

int exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description)
{
    auto logger = spdlog::get("lua");
    if (maybe_exception)
    {
        const auto& ex = *maybe_exception;
        logger->critical("Lua exception thrown: [[[{}]]]", ex.what());
    }
    else
    {
        logger->critical("Lua error thrown: [[[{}]]]", std::string_view(description.data(), description.size()));
        std::cout << std::endl;
    }

    return sol::stack::push(L, description);
}

} // namespace cs::lua
