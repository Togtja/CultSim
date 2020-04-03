#include "scenario_scene.h"
#include "common_helpers.h"
#include "constants.h"
#include "debug/native_collectors.h"
#include "entity/actions/action.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/tags.h"
#include "entity/factory.h"
#include "entity/memories/resource_location.h"
#include "entity/systems/action.h"
#include "entity/systems/health.h"
#include "entity/systems/mitigation.h"
#include "entity/systems/movement.h"
#include "entity/systems/need.h"
#include "entity/systems/rendering.h"
#include "entity/systems/reproduction.h"
#include "entity/systems/requirement.h"
#include "entity/systems/sensor.h"
#include "entity/systems/timer.h"
#include "gfx/renderer.h"
#include "input/input_handler.h"
#include "preferences.h"
#include "random_engine.h"
#include "scene_manager.h"
#include "scenes/pausemenu_scene.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <random>

#include <gfx/ImGUI/imgui.h>
#include <spdlog/spdlog.h>

extern ImFont* g_header_font;
extern ImFont* g_light_font;

namespace cs
{
ScenarioScene::ScenarioScene(std::string_view scenario)
{
    m_scenario.script_path = scenario;
}

ScenarioScene::ScenarioScene(lua::Scenario scenario) : m_scenario(std::move(scenario))
{
}

void ScenarioScene::initialize_simulation()
{
    /** Run all initialization functions from Lua and required once for this scenario */
    m_context->lua_state["random"] = &m_rng;
    bind_actions_for_scene();
    bind_available_lua_events();
    bind_scenario_lua_functions();
    m_scenario = lua::quick_load_scenario(m_context->lua_state, m_scenario.script_path);
    gfx::get_renderer().set_camera_bounds(m_scenario.bounds);
    gfx::get_renderer().set_camera_position({0.f, 0.f, 1.f});

    /** Set up context variables in EnTT */
    m_registry.set<EntitySelectionHelper>();

    /** Call lua init function for this scenario */
    m_scenario.init();

    /** TODO: Read in data samplers from Lua */
    m_data_collector.set_sampling_rate(m_scenario.sampling_rate);
    m_data_collector.add_collector<debug::CollectorLivingEntities>(m_registry);
    m_data_collector.add_collector<debug::CollectorAverageHealth>(m_registry);
    m_data_collector.add_collector<debug::CollectorMouse>(true);
    m_data_collector.add_collector<debug::CollectorMouse>(false);

    /** Add systems specified by scenario */
    for (const auto& system : m_scenario.systems)
    {
        auto type = entt::resolve(entt::hashed_string(system.c_str()));
        if (type)
        {
            auto meta = type.construct(
                system::SystemContext{&m_registry, &m_dispatcher, &m_rng, &m_scenario, &m_mt_executor, &m_context->lua_state});
            system::ISystem& temp_ref = meta.cast<system::ISystem>();
            m_active_systems.emplace_back(temp_ref.clone());
            m_active_systems.back()->initialize();
        }
        else
        {
            spdlog::get("scenario")->warn("adding system \"{}\" that is unknown", system);
        }
    }

    /** Enforce the use of a rendering system */
    m_draw_systems.emplace_back(
        new system::Rendering(system::SystemContext{&m_registry, &m_dispatcher, &m_rng, &m_scenario, &m_mt_executor}));

    /** Notify the scenario is loaded */
    m_dispatcher.enqueue<event::ScenarioLoaded>();
}

void ScenarioScene::clean_simulation()
{
    m_data_collector.save_to_file(m_scenario.name + "_data", true);

    m_registry.clear();
    m_simtime   = 0.f;
    m_timescale = 1.f;
    m_data_collector.clear();
    m_notifications.clear();

    /** Deinitialize systems and then clear them */
    for (auto& system : m_active_systems)
    {
        system->deinitialize();
    }
    m_active_systems.clear();

    for (auto& system : m_draw_systems)
    {
        system->initialize();
    }
    m_draw_systems.clear();

    /** Clean up event handlers and binders */
    for (auto& handler : m_lua_event_handlers)
    {
        handler->connection.release();
    }
    m_lua_event_handlers.clear();
    m_lua_ebinder.clear();
}

void ScenarioScene::reset_simulation()
{
    clean_simulation();
    initialize_simulation();
}

void ScenarioScene::on_enter()
{
    initialize_simulation();

    m_resolution = std::get<glm::ivec2>(m_context->preferences->get_resolution().value);
    m_context->preferences->on_preference_changed.connect<&ScenarioScene::handle_preference_changed>(this);
    input::get_input().add_context(input::EKeyContext::ScenarioScene);
}

void ScenarioScene::on_exit()
{
    input::get_input().remove_context(input::EKeyContext::ScenarioScene);
    m_context->preferences->on_preference_changed.disconnect<&ScenarioScene::handle_preference_changed>(this);

    clean_simulation();
}

bool ScenarioScene::update(float dt)
{
    static auto b_tex  = gfx::get_renderer().sprite().get_texture("sprites/background_c.png");
    b_tex.scale        = 100;
    b_tex.material_idx = MATERIAL_IDX_NOSPEC;

    /** Draw background crudely */
    for (int i = -m_scenario.bounds.x / 100; i <= m_scenario.bounds.x / 100; i++)
    {
        for (int j = -m_scenario.bounds.y / 100; j <= m_scenario.bounds.y / 100; j++)
        {
            gfx::get_renderer().sprite().draw(glm::vec3(i * 100.f, j * 100.f, 0.f), glm::vec3(0.05f, 0.15f, 0.0f), b_tex);
        }
    }

    // TODO : Move to input action response
    update_entity_hover();

    setup_docking_ui();
    ImGui::Begin(m_scenario.name.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar);
    draw_scenario_information_ui();
    draw_time_control_ui();
    draw_notifications(dt);
    draw_selected_entity_information_ui();

    /** Sample data */
    m_data_collector.show_ui();
    auto time_step = m_timescale;
    if (m_paused)
    {
        time_step = 0;
    }
    for (int i = 0; i < time_step; ++i)
    {
        m_data_collector.update(dt);

        /** Update systems */
        for (auto&& system : m_active_systems)
        {
            system->update(dt);
        }

        /** Deal with long running tasks, then events */
        m_scheduler.update(dt);
        m_dispatcher.update();
        m_scenario.update(dt);
    }

    /** It's supposed to be three of these here, do not change - not a bug */

    ImGui::End();
    ImGui::End();

    if (ImGui::Button("Restart Simulation"))
    {
        reset_simulation();
    }

    return false;
}

bool ScenarioScene::draw()
{
    for (const auto& system : m_draw_systems)
    {
        /** Pass 0, since draw systems don't need time */
        system->update(0.f);
    }

    auto& r_debug = gfx::get_renderer().debug();
    r_debug.draw_rect({0.f, 0.f, 0.f}, m_scenario.bounds * 2.f, {1.f, 1.f, 1.f});
    m_scenario.draw();
    return false;
}

void ScenarioScene::bind_actions_for_scene()
{
    /** Select entity on click */
    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::SelectEntity, [this] {
        auto&& select_helper = m_registry.ctx<EntitySelectionHelper>();

        if (m_registry.valid(select_helper.selected_entity))
        {
            m_registry.remove<entt::tag<"selected"_hs>>(select_helper.selected_entity);
        }

        select_helper.selected_entity = select_helper.hovered_entity;

        if (m_registry.valid(select_helper.selected_entity))
        {
            m_registry.assign<entt::tag<"selected"_hs>>(select_helper.selected_entity);
        }
    });

    /** Move to selected entity */
    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::FollowEntity, [this](float dt) {
        auto&& select_helper = m_registry.ctx<EntitySelectionHelper>();
        if (!m_registry.valid(select_helper.selected_entity))
        {
            return;
        }
        const auto& pos_comp = m_registry.get<component::Position>(select_helper.selected_entity);

        // TODO: Steal UE4 FInterpretTo
        auto pos =
            glm::mix(gfx::get_renderer().get_camera_position2d(), glm::vec2{pos_comp.position.x, pos_comp.position.y}, 0.05f);

        gfx::get_renderer().set_camera_position_2d(pos);
    });

    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::PauseMenu, [this] {
        m_context->scene_manager->push<PauseMenuScene>();
    });

    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::SpeedUp, [this]() {
        m_timescale = std::clamp(m_timescale *= 2, 1, 100);
    });

    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::SpeedDown, [this]() {
        m_timescale = std::clamp(m_timescale /= 2, 1, 100);
    });

    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::Pause, [this]() { m_paused = !m_paused; });

    /** Camera Controls */
    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::MoveUp, [](float dt) {
        gfx::get_renderer().move_camera(glm::vec3(0.f, 1.f, 0.f) * dt * 200.f);
    });
    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::MoveLeft, [](float dt) {
        gfx::get_renderer().move_camera(glm::vec3(-1.f, 0.f, 0.f) * dt * 200.f);
    });
    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::MoveDown, [](float dt) {
        gfx::get_renderer().move_camera(glm::vec3(0.f, -1.f, 0.f) * dt * 200.f);
    });
    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::MoveRight, [](float dt) {
        gfx::get_renderer().move_camera(glm::vec3(1.f, 0.f, 0.f) * dt * 200.f);
    });
    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::ZoomIn, [] {
        gfx::get_renderer().move_camera({0.f, 0.f, -.05f});
    });
    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::ZoomOut, [] {
        gfx::get_renderer().move_camera({0.f, 0.f, .05f});
    });
}

void ScenarioScene::bind_available_lua_events()
{
    m_lua_ebinder = {{"ArrivedAtDestination", &lua_binder<event::ArrivedAtDestination>},
                     {"ScenarioLoaded", &lua_binder<event::ScenarioLoaded>}};
}

void ScenarioScene::bind_scenario_lua_functions()
{
    /** Helpful to make following code shorter and more readable, copying is fine here, it's just a pointer */
    auto lua = m_context->lua_state;

    /** Bind component type identifiers to the component table */
    sol::table component      = lua.create_table("component");
    component["position"]     = entt::type_info<component::Position>::id();
    component["movement"]     = entt::type_info<component::Movement>::id();
    component["sprite"]       = entt::type_info<component::Sprite>::id();
    component["vision"]       = entt::type_info<component::Vision>::id();
    component["tag"]          = entt::type_info<component::Tags>::id();
    component["need"]         = entt::type_info<component::Need>::id();
    component["reproduction"] = entt::type_info<component::Reproduction>::id();
    component["strategy"]     = entt::type_info<component::Strategy>::id();
    component["health"]       = entt::type_info<component::Health>::id();
    component["memory"]       = entt::type_info<component::Memory>::id();
    component["attack"]       = entt::type_info<component::Attack>::id();
    component["inventory"]    = entt::type_info<component::Inventory>::id();

    /** Get component from Lua */
    sol::table cultsim = lua.create_table("cultsim");
    cultsim.set_function("get_component", [this](sol::this_state s, entt::entity e, uint32_t id) -> sol::object {
        switch (id)
        {
            case entt::type_info<component::Position>::id():
                if (m_registry.try_get<component::Position>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Position>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Movement>::id():
                if (m_registry.try_get<component::Movement>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Movement>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Sprite>::id():
                if (m_registry.try_get<component::Sprite>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Sprite>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Vision>::id():
                if (m_registry.try_get<component::Vision>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Vision>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Tags>::id():
                if (m_registry.try_get<component::Tags>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Tags>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Need>::id():
                if (m_registry.try_get<component::Need>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Need>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Reproduction>::id():
                if (m_registry.try_get<component::Reproduction>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Reproduction>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Strategy>::id():
                if (m_registry.try_get<component::Strategy>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Strategy>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Health>::id():
                if (m_registry.try_get<component::Health>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Health>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Memory>::id():
                if (m_registry.try_get<component::Memory>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Memory>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Attack>::id():
                if (m_registry.try_get<component::Attack>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Attack>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Inventory>::id():
                if (m_registry.try_get<component::Inventory>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Inventory>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            default:
                spdlog::critical("can not find the component [{}] you are looking for", id);
                return sol::nil;
                break;
        }
    });
    cultsim.set_function("remove_component", [this](entt::entity e, uint32_t id) {
        switch (id)
        {
            case entt::type_info<component::Position>::id(): m_registry.remove_if_exists<component::Position>(e);
            case entt::type_info<component::Movement>::id(): m_registry.remove_if_exists<component::Movement>(e); break;
            case entt::type_info<component::Sprite>::id(): m_registry.remove_if_exists<component::Sprite>(e); break;
            case entt::type_info<component::Vision>::id(): m_registry.remove_if_exists<component::Vision>(e); break;
            case entt::type_info<component::Tags>::id(): m_registry.remove_if_exists<component::Tags>(e); break;
            case entt::type_info<component::Need>::id(): m_registry.remove_if_exists<component::Need>(e); break;
            case entt::type_info<component::Reproduction>::id(): m_registry.remove_if_exists<component::Reproduction>(e); break;
            case entt::type_info<component::Strategy>::id(): m_registry.remove_if_exists<component::Strategy>(e); break;
            case entt::type_info<component::Health>::id(): m_registry.remove_if_exists<component::Health>(e); break;
            case entt::type_info<component::Memory>::id(): m_registry.remove_if_exists<component::Memory>(e); break;
            case entt::type_info<component::Inventory>::id(): m_registry.remove_if_exists<component::Inventory>(e); break;
            default: break;
        }
    });
    cultsim.set_function("add_component", [this](sol::this_state s, entt::entity e, uint32_t id) -> sol::object {
        switch (id)
        {
            case entt::type_info<component::Position>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Position>(e));
                break;
            case entt::type_info<component::Movement>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Movement>(e));
                break;
            case entt::type_info<component::Sprite>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Sprite>(e));
                break;
            case entt::type_info<component::Vision>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Vision>(e));
                break;
            case entt::type_info<component::Tags>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Tags>(e));
                break;
            case entt::type_info<component::Need>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Need>(e));
                break;
            case entt::type_info<component::Reproduction>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Reproduction>(e));
                break;
            case entt::type_info<component::Strategy>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Strategy>(e));
                break;
            case entt::type_info<component::Health>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Health>(e));
                break;
            case entt::type_info<component::Memory>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Memory>(e));
                break;
            case entt::type_info<component::Inventory>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Inventory>(e));
                break;
            default: break;
        }
    });

    /** Helper action to modify an entity need */
    cultsim.set_function("modify_need", [this](entt::entity e, ETag need_tags, float delta) {
        if (auto* needs = m_registry.try_get<component::Need>(e); needs)
        {
            for (auto& need : needs->needs)
            {
                if (need.tags & need_tags)
                {
                    need.status += delta;
                }
            }
        }
    });

    cultsim.set_function("add_to_inventory", [this](entt::entity owner, entt::entity target) {
        if (auto inventory = m_registry.try_get<component::Inventory>(owner); inventory)
        {
            if (inventory->size < inventory->max_size)
            {
                auto tags = m_registry.try_get<component::Tags>(target);
                m_dispatcher.enqueue<event::PickedUpEntity>(event::PickedUpEntity{owner, target, tags->tags});
                tags->tags = ETag(tags->tags | TAG_Inventory);
                inventory->contents.push_back(target);
            }
        }
    });

    cultsim.set_function("remove_from_inventory", [this](entt::entity owner, entt::entity target) {
        if (auto inventory = m_registry.try_get<component::Inventory>(owner); inventory)
        {
            int i = 0;
            for (auto& content : inventory->contents)
            {
                if (content == target)
                {
                    inventory->contents.erase(inventory->contents.begin() + i);
                }
                i++;
            }
        }
    });

    /** Apply Damage */
    cultsim.set_function("apply_basic_damage", [this](entt::entity e, float damage) {
        if (auto* health = m_registry.try_get<component::Health>(e); health)
        {
            health->health -= damage;
        }
    });

    /* Function to allow lua to connect to events */
    cultsim.set_function("connect", [this](const std::string& event_name, sol::function func) {
        auto handle        = std::make_unique<LuaEventHandle>();
        handle->func       = func;
        handle->connection = m_lua_ebinder.at(event_name)(m_dispatcher, handle->func);
        m_lua_event_handlers.emplace_back(std::move(handle));
    });

    /** Send notification */
    cultsim.set_function("notify", [this](std::string title, std::string information, float duration = 4.f) {
        m_notifications.push_back({std::move(title), std::move(information), -duration});
    });

    /** Spawn entity functions */
    cultsim.set_function("spawn", [this](const std::string& entity_name) {
        const auto& final_path = m_scenario.script_path + "/entities/" + entity_name + ".lua";
        return spawn_entity(m_registry, m_context->lua_state, final_path);
    });

    cultsim.set_function("spawn_at", [this](const std::string& entity_name, glm::vec2 position) {
        const auto& final_path = m_scenario.script_path + "/entities/" + entity_name + ".lua";
        return spawn_entity(m_registry, m_context->lua_state, final_path, position);
    });

    /** Destroy entity */
    cultsim.set_function("kill", [this](entt::entity e) {
        if (e == entt::null)
        {
            spdlog::get("agent")->critical("trying to kill a null agent");
            return;
        }
        m_registry.assign<component::Delete>(e);
        auto tags = m_registry.try_get<component::Tags>(e);
        if (tags)
        {
            tags->tags = static_cast<ETag>(tags->tags | ETag::TAG_Delete);
        }
    });

    /** Check entity validity */
    cultsim.set_function("is_valid", [this](entt::entity e) { return m_registry.valid(e); });

    /** Impregnate */
    cultsim.set_function("impregnate", [this](sol::this_state s, entt::entity father, entt::entity mother) {
        /** Figure out "type" of mother and spawn a child based on that */

        /** Give a Pregnancy component to the mother */
        auto* rc_m = m_registry.try_get<component::Reproduction>(mother);
        auto* rc_f = m_registry.try_get<component::Reproduction>(father);
        if (rc_m && rc_f)
        {
            if (!m_rng.trigger(rc_m->fertility) || !m_rng.trigger(rc_f->fertility))
            {
                // N0 pregnancy
                return;
            }
            if (auto* again = m_registry.try_get<component::Pregnancy>(mother); again)
            {
                // Can't get pregnant twice
                return;
            }
            cs::component::Pregnancy* preg;
            if (rc_f->incubator == component::Reproduction::ESex::Female)
            {
                preg = &m_registry.assign<component::Pregnancy>(mother);
                if (rc_m->children_deviation > 0)
                {
                    preg->children_in_pregnancy = std::round(m_rng.normal(rc_m->mean_children_pp, rc_m->children_deviation));
                }
                else
                {
                    preg->children_in_pregnancy = rc_f->mean_children_pp;
                }
                if (rc_m->gestation_deviation > 0)
                {
                    preg->gestation_period = std::round(m_rng.normal(rc_m->average_gestation_period, rc_m->gestation_deviation));
                }
                else
                {
                    preg->gestation_period = rc_m->average_gestation_period;
                }
                preg->parents.first  = mother;
                preg->parents.second = father;
            }
            else
            {
                preg = &m_registry.assign<component::Pregnancy>(father);
                if (rc_f->children_deviation > 0)
                {
                    preg->children_in_pregnancy = m_rng.normal(rc_f->mean_children_pp, rc_f->children_deviation);
                }
                else
                {
                    preg->children_in_pregnancy = rc_f->mean_children_pp;
                }
                if (rc_f->gestation_deviation > 0)
                {
                    preg->gestation_period = m_rng.normal(rc_f->average_gestation_period, rc_f->gestation_deviation);
                }
                else
                {
                    preg->gestation_period = rc_f->average_gestation_period;
                }
                // Here the incubator is the dad
                preg->parents.second = mother;
                preg->parents.first  = father;
            }
            if (preg->children_in_pregnancy < 1)
            {
                preg->children_in_pregnancy = 1;
            }
        }
    });
}

void ScenarioScene::setup_docking_ui()
{
    /** Set up Scenario Dock Space first */
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scenario##DockingWindow",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking);
    ImGui::PopStyleVar();

    /** Create docking space inside of docking window */
    ImGuiID dockspace_id = ImGui::GetID("Scenario##Docking");
    ImGui::DockSpace(dockspace_id,
                     {0.f, 0.f},
                     ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode |
                         ImGuiDockNodeFlags_AutoHideTabBar);
}

void ScenarioScene::draw_scenario_information_ui()
{
    /** Title and description */
    ImGui::PushFont(g_header_font);
    ImGui::TextColored({1.f, 0.843, 0.f, 1.f}, "%s", m_scenario.name.c_str());
    ImGui::PopFont();
    ImGui::Indent();
    ImGui::TextWrapped("%s", m_scenario.description.c_str());
    ImGui::Unindent();
    ImGui::Separator();

    /** Stat Bar */
    ImGui::PushFont(g_light_font);
    ImGui::Spacing();
    ImGui::TextColored({0.0, 0.749, 1., 1.}, "FPS: %5.1f", ImGui::GetIO().Framerate);
    ImGui::SameLine();
    ImGui::TextColored({0.0, 0.98, 0.604, 1.0}, "Entities: %u", static_cast<uint32_t>(m_registry.view<component::Tags>().size()));
    ImGui::SameLine();
    if (m_paused)
    {
        ImGui::TextColored({1., 0.627, 0.478, 1.}, "Runtime: %4.1f (%dx)", m_simtime, 0);
    }
    else
    {
        ImGui::TextColored({1., 0.627, 0.478, 1.}, "Runtime: %4.1f (%dx)", m_simtime, m_timescale);
    }
    ImGui::Spacing();
    ImGui::PopFont();
    ImGui::Separator();
}

void ScenarioScene::draw_notifications(float dt)
{
    if (m_notifications.empty())
    {
        return;
    }

    /** Set up notification window area */
    ImGui::SetNextWindowPos({m_resolution.x - 50.f, 50.f}, ImGuiCond_Always, {1, 0});
    ImGui::SetNextWindowSize({260.f, m_notifications.size() * 60.f});
    ImGui::SetNextWindowBgAlpha(0.6f);
    ImGui::Begin("Notifications", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);

    /** Draw all notifications */
    for (auto itr = m_notifications.rbegin(); itr != m_notifications.rend(); ++itr)
    {
        auto& notification = *itr;
        notification.time_shown += dt;

        /** Show current */
        if (ImGui::BeginChild("Notification", {250, 0}, true, ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::Text("%s", notification.title.c_str());
            ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s", notification.information.c_str());
        }
        ImGui::EndChild();
    }

    ImGui::End();

    /** Get rid of old notifications */
    m_notifications.erase(
        std::remove_if(m_notifications.begin(), m_notifications.end(), [](const Notification& n) { return n.time_shown > 0.f; }),
        m_notifications.end());
}

void ScenarioScene::draw_time_control_ui()
{
    ImGui::SetNextWindowPos({m_resolution.x / 2.f, 0.f}, 0, {0.5f, 0.f});
    ImGui::SetNextWindowSize({360, 64});
    ImGui::Begin("Time Control", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::Text("Time Scaling");
    if (ImGui::Button("||", {36, 24}))
    {
        m_paused = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(">", {36, 24}))
    {
        m_timescale = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button(">>", {36, 24}))
    {
        m_timescale = 2;
    }
    ImGui::SameLine();
    if (ImGui::Button("5x", {36, 24}))
    {
        m_timescale = 5;
    }
    ImGui::SameLine();
    if (ImGui::Button("10x", {36, 24}))
    {
        m_timescale = 10;
    }
    ImGui::SameLine();
    if (ImGui::Button("20x", {36, 24}))
    {
        m_timescale = 20;
    }
    ImGui::SameLine();
    if (ImGui::Button("50x", {36, 24}))
    {
        m_timescale = 50;
    }
    ImGui::End();
}

void ScenarioScene::draw_selected_entity_information_ui()
{
    const auto& selection_info = m_registry.ctx<EntitySelectionHelper>();
    if (!m_registry.valid(selection_info.selected_entity))
    {
        return;
    }

    const auto& [needs, health, strategy, reproduction, timer, tags, memories, preg] =
        m_registry.try_get<component::Need,
                           component::Health,
                           component::Strategy,
                           component::Reproduction,
                           component::Timer,
                           component::Tags,
                           component::Memory,
                           component::Pregnancy>(selection_info.selected_entity);

    ImGui::SetNextWindowPos({250.f, 250.f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({400.f, 600.f}, ImGuiCond_FirstUseEver);
    ImGui::Begin("Agent Information");

    auto text = fmt::format("Ola Normann nr {}", static_cast<int64_t>(selection_info.selected_entity));
    ImGui::Text(text.c_str());

    if (health)
    {
        ImGui::PushFont(g_header_font);
        ImGui::TextColored({.486f, .988f, 0.f, 1.f}, "%3.0f/100 HP", health->health);
        ImGui::PopFont();
    }

    if (reproduction)
    {
        if (preg && preg->is_egg)
        {
            ImGui::Text("I am a hatching egg");
        }
        else
        {
            ImGui::Text("I am a %s, and I have %d children.",
                        (reproduction->sex == component::Reproduction::ESex::Male ? "Male" : "Female"),
                        reproduction->number_of_children);
        }
    }

    if (preg)
    {
        if (preg->is_egg)
        {
            ImGui::ProgressBar(preg->time_since_start / preg->gestation_period, {-1, 0}, "Hatching");
        }
        else
        {
            ImGui::ProgressBar(preg->time_since_start / preg->gestation_period, {-1, 0}, "Pregnancy");
        }
    }

    if (strategy)
    {
        if (!strategy->staged_strategies.empty())
        {
            ImGui::Text("Currently: %s", strategy->staged_strategies.back().name.c_str());
            if (!strategy->staged_strategies.back().actions.empty())
            {
                const auto& action = strategy->staged_strategies.back().actions.back();
                if (action.time_spent > 0.f)
                {
                    ImGui::Indent();
                    ImGui::Text("%s", action.name.c_str());
                    ImGui::TextColored({0.f, 0.749, 1.0, 1.0}, " (%4.2f/%4.2f)", action.time_spent, action.time_to_complete);
                    ImGui::SameLine();
                    ImGui::ProgressBar(action.time_spent / action.time_to_complete, {-1, 0}, "Progress");
                    ImGui::Unindent();
                }
            }
        }
        else
        {
            ImGui::Text("Currently: Doing nothing");
        }
    }

    if (needs)
    {
        if (ImGui::BeginTable("Entity Needs", 2))
        {
            ImGui::TableSetupColumn("Need");
            ImGui::TableSetupColumn("Status");
            ImGui::TableAutoHeaders();
            for (const auto& need : needs->needs)
            {
                ImGui::TableNextCell();
                ImGui::Text("%s", need.name.c_str());
                ImGui::TableNextCell();
                ImGui::Text("%3.1f", need.status);
            }
            ImGui::EndTable();
        }
    }

    if (tags)
    {
        ImGui::Text("Tags: %s", tag_to_string(tags->tags).c_str());
    }

    if (timer)
    {
        ImGui::Text("Timer: %d cycles left", timer->number_of_loops);
        ImGui::ProgressBar(timer->time_spent / timer->time_to_complete, ImVec2{-1, 0}, "Progress");
    }

    if (memories)
    {
        if (!memories->memory_container.empty())
        {
            if (ImGui::BeginTable("Entity Memories", 2))
            {
                ImGui::TableSetupColumn("Tags");
                ImGui::TableSetupColumn("Size");
                ImGui::TableAutoHeaders();
                for (auto& memory : memories->memory_container)
                {
                    ImGui::TableNextCell();
                    ImGui::Text("%s", tag_to_string(memory.memory_tags).c_str());
                    ImGui::TableNextCell();
                    ImGui::Text("%zu", memory.memory_storage.size());
                    if (memory.memory_tags & ETag::TAG_Location)
                    {
                        if (ImGui::BeginTable(tag_to_string(memory.memory_tags).c_str(), 2))
                        {
                            ImGui::TableSetupColumn("Age");
                            ImGui::TableSetupColumn("Entity Count");
                            ImGui::TableAutoHeaders();
                            for (auto& mem : memory.memory_storage)
                            {
                                ImGui::TableNextCell();
                                ImGui::Text("%u", mem->m_time_since_creation);
                                ImGui::TableNextCell();
                                ImGui::Text("%u", dynamic_cast<memory::ResourceLocation*>(mem.get())->m_number_of_entities);
                            }
                            ImGui::EndTable();
                        }
                    }
                }
                ImGui::EndTable();
            }
        }
    }

    ImGui::End();
}

void ScenarioScene::update_entity_hover()
{
    auto&& selection_helper = m_registry.ctx<EntitySelectionHelper>();
    auto cursor_pos         = input::get_input().get_mouse_pos();
    auto world_pos = gfx::get_renderer().screen_to_world_pos({cursor_pos.x, m_resolution.y - cursor_pos.y}, m_resolution);

    auto hover_view = m_registry.view<component::Position, component::Sprite>();
    if (m_registry.valid(selection_helper.hovered_entity))
    {
        m_registry.remove<entt::tag<"hovered"_hs>>(selection_helper.hovered_entity);
    }

    /** Reset hover status */
    selection_helper.hovered_entity = entt::null;
    for (auto e : hover_view)
    {
        if (glm::distance(world_pos, hover_view.get<component::Position>(e).position) < 10.f)
        {
            selection_helper.hovered_entity = e;
            break;
        }
    };

    if (m_registry.valid(selection_helper.hovered_entity))
    {
        m_registry.assign<entt::tag<"hovered"_hs>>(selection_helper.hovered_entity);
    }

    ImGui::Text("Screen: %d | %d", cursor_pos.x, cursor_pos.y);
    ImGui::Text("World: %.2f | %.2f | %.2f", world_pos.x, world_pos.y, world_pos.z);
    ImGui::Text("Hovering: %u | Selected: %u",
                static_cast<uint32_t>(selection_helper.hovered_entity),
                static_cast<uint32_t>(selection_helper.selected_entity));
}

void ScenarioScene::handle_preference_changed(const Preference& before, const Preference& after)
{
    if (after.name == "Resolution")
    {
        m_resolution = std::get<glm::ivec2>(after.value);
    }
}
} // namespace cs
