#include "scenario_scene.h"
#include "ai/path_finding.h"
#include "common_helpers.h"
#include "constants.h"
#include "debug/native_collectors.h"
#include "entity/actions/action.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/tags.h"
#include "entity/effect.h"
#include "entity/events.h"
#include "entity/factory.h"
#include "entity/memories/resource_location.h"
#include "entity/systems/action.h"
#include "entity/systems/health.h"
#include "entity/systems/lua_system.h"
#include "entity/systems/mitigation.h"
#include "entity/systems/movement.h"
#include "entity/systems/need.h"
#include "entity/systems/relationship.h"
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
ScenarioScene::ScenarioScene(std::string_view scenario, const lua::ScenarioLoadPreferences& preferences) :
    m_scenario_preferences(preferences)
{
    m_scenario.script_path = scenario;
}

void ScenarioScene::initialize_simulation()
{
    m_rng = RandomEngine(m_scenario_preferences.seed);

    /** Run all initialization functions from Lua and required once for this scenario */
    bind_actions_for_scene();
    bind_available_lua_events();
    bind_scenario_lua_functions();
    m_context->lua_state["random"] = &m_rng;
    m_scenario                     = lua::quick_load_scenario(m_context->lua_state, m_scenario.script_path);
    gfx::get_renderer().set_camera_bounds(m_scenario.bounds);
    gfx::get_renderer().set_camera_position({0.f, 0.f, 1.f});

    /** Set up context variables in EnTT */
    m_registry.set<EntitySelectionHelper>();
    m_registry.set<RandomEngine*>(&m_rng);

    /** Add lua data collectors */
    m_data_collector.set_sampling_rate(m_scenario.sampling_rate);
    for (const auto& lua_collector : m_scenario.data_collectors)
    {
        m_data_collector.add_collector<debug::LuaCollector>(lua_collector);
    }

    m_data_collector.add_collector<debug::CollectorLivingEntities>(m_registry);
    m_data_collector.add_collector<debug::CollectorAverageHealth>(m_registry);
    m_data_collector.add_collector<debug::CollectorMouse>(true, m_resolution);
    m_data_collector.add_collector<debug::CollectorMouse>(false, m_resolution);

    /** Add systems specified by scenario */
    for (const auto& system : m_scenario.systems)
    {
        auto ctx = system::SystemContext{&m_registry, &m_dispatcher, &m_rng, &m_scenario, &m_mt_executor, &m_context->lua_state};

        if (system.get_type() == sol::type::string)
        {
            auto type = entt::resolve(entt::hashed_string(system.as<std::string>().c_str()));
            if (type)
            {
                auto meta                 = type.construct(ctx);
                system::ISystem& temp_ref = meta.cast<system::ISystem>();
                m_active_systems.emplace_back(temp_ref.clone());
                m_active_systems.back()->initialize();
            }
            else
            {
                spdlog::get("lua")->warn("attempt to add unknown native system: {}", system.as<std::string>());
            }
        }
        else if (system.get_type() == sol::type::table)
        {
            m_active_systems.emplace_back(new system::LuaSystem(ctx, system.as<sol::table>()));
            m_active_systems.back()->initialize();
        }
        else
        {
            spdlog::get("lua")->error("failed to spawn Lua system. ensure parameters are correct to scenario.systems.");
        }
    }

    /** Call lua init function for this scenario */
    m_scenario.init();

    /** If there are any default Traits, run their affects and add them */
    auto per_view = m_registry.view<component::Traits>();
    per_view.each([](entt::entity e, component::Traits& traits) {
        /** Add default traits to acquired traits */
        for (const auto& trait : traits.start_traits)
        {
            traits.acquired_traits.push_back(trait);
        }

        /** Run their effects */
        effect::affect_traits(e, traits);
    });

    /** Render only if enabled */
    if (m_scenario_preferences.enable_rendering)
    {
        m_draw_systems.emplace_back(
            new system::Rendering(system::SystemContext{&m_registry, &m_dispatcher, &m_rng, &m_scenario, &m_mt_executor}));
        m_draw_systems.back()->initialize();
    }

    /** Notify the scenario is loaded */
    m_dispatcher.enqueue<event::ScenarioLoaded>();
}

void ScenarioScene::clean_simulation()
{
    m_data_collector.save_to_file(m_scenario.name + "_data", true);

    m_registry.clear();
    m_simtime   = 0.0;
    m_timescale = 1;
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
        system->deinitialize();
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
    m_resolution = std::get<glm::ivec2>(m_context->preferences->get_resolution().value);
    m_context->preferences->on_preference_changed.connect<&ScenarioScene::handle_preference_changed>(this);
    m_name_generator.initialize(m_context->lua_state["origins"].get<sol::table>());
    input::get_input().add_context(input::EKeyContext::ScenarioScene);

    initialize_simulation();
}

void ScenarioScene::on_exit()
{
    input::get_input().remove_context(input::EKeyContext::ScenarioScene);
    m_context->preferences->on_preference_changed.disconnect<&ScenarioScene::handle_preference_changed>(this);

    clean_simulation();
}

bool ScenarioScene::update(float dt)
{
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

    gfx::get_renderer().update_program_info(m_simtime, input::get_input().get_mouse_pos(), m_resolution);
    for (int i = 0; i < time_step; ++i)
    {
        m_data_collector.update(dt);
        m_simtime += dt;

        /** Update systems */
        for (auto&& system : m_active_systems)
        {
            system->update(dt);
        }

        /** Deal with long running tasks, then events */
        m_scenario.update(dt);
        m_scheduler.update(dt);
        m_dispatcher.update();
    }

    /** Deal with ImGui updates once per tick */
    for (auto&& system : m_active_systems)
    {
        system->update_imgui();
    }
    for (auto&& system : m_draw_systems)
    {
        system->update_imgui();
    }

    /** It's supposed to be three (two?) of these here, do not change - not a bug */
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
    /** Draw background crudely */
    static auto b_tex  = gfx::get_renderer().sprite().get_texture("sprites/background_c.png");
    b_tex.scale        = 100;
    b_tex.material_idx = MATERIAL_IDX_NOSPEC;
    for (int i = -m_scenario.bounds.x / 100; i <= m_scenario.bounds.x / 100; i++)
    {
        for (int j = -m_scenario.bounds.y / 100; j <= m_scenario.bounds.y / 100; j++)
        {
            gfx::get_renderer().sprite().draw(glm::vec3(i * 100.f, j * 100.f, 0.f), glm::vec3(0.15f, 0.15f, 0.15f), b_tex);
        }
    }

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

    /** Change render mode */
    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::SetMode2D, [] {
        gfx::get_renderer().set_render_mode(gfx::ERenderingMode::Render_2D);
    });

    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::SetMode3D, [] {
        gfx::get_renderer().set_render_mode(gfx::ERenderingMode::Render_3D);
    });

    /** Move to selected entity */
    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::FollowEntity, [this](float dt) {
        auto&& select_helper = m_registry.ctx<EntitySelectionHelper>();
        if (!m_registry.valid(select_helper.selected_entity))
        {
            return;
        }
        const auto& pos_comp = m_registry.get<component::Position>(select_helper.selected_entity);

        const auto pos =
            glm::mix(gfx::get_renderer().get_camera_position2d(), glm::vec2{pos_comp.position.x, pos_comp.position.y}, 10.f * dt);

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
                     {"ScenarioLoaded", &lua_binder<event::ScenarioLoaded>},
                     {"SensedEntity", &lua_binder<event::SensedEntity>},
                     {"NeedBecameCritical", &lua_binder<event::NeedBecameCritical>},
                     {"NeedNoLongerCritical", &lua_binder<event::NeedNoLongerCritical>},
                     {"SwitchNeedContext", &lua_binder<event::SwitchNeedContext>},
                     {"FinishedRequirement", &lua_binder<event::FinishedRequirement>},
                     {"RequirementFailure", &lua_binder<event::RequirementFailure>},
                     {"CreatedMemory", &lua_binder<event::CreatedMemory>},
                     {"EntityDeleted", &lua_binder<event::EntityDeleted>},
                     {"EntitySpawned", &lua_binder<event::EntitySpawned>},
                     {"EntityBorn", &lua_binder<event::EntityBorn>}};
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
    ImGui::TextColored({0.0, 0.98, 0.604, 1.0}, "Entities: %u", static_cast<uint32_t>(m_registry.size()));
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
        m_paused    = false;
        m_timescale = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button("2x", {36, 24}))
    {
        m_paused    = false;
        m_timescale = 2;
    }
    ImGui::SameLine();
    if (ImGui::Button("5x", {36, 24}))
    {
        m_paused    = false;
        m_timescale = 5;
    }
    ImGui::SameLine();
    if (ImGui::Button("10x", {36, 24}))
    {
        m_paused    = false;
        m_timescale = 10;
    }
    ImGui::SameLine();
    if (ImGui::Button("20x", {36, 24}))
    {
        m_paused    = false;
        m_timescale = 20;
    }
    ImGui::SameLine();
    if (ImGui::Button("50x", {36, 24}))
    {
        m_paused    = false;
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

    const auto& [name, needs, goal, action, health, strategy, reproduction, timer, tags, memories, preg, traits, relship] =
        m_registry.try_get<component::Name,
                           component::Need,
                           component::Goal,
                           component::Action,
                           component::Health,
                           component::Strategy,
                           component::Reproduction,
                           component::Timer,
                           component::Tags,
                           component::Memory,
                           component::Pregnancy,
                           component::Traits,
                           component::Relationship>(selection_info.selected_entity);

    ImGui::SetNextWindowPos({250.f, 250.f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({400.f, 600.f}, ImGuiCond_FirstUseEver);
    ImGui::Begin("Agent Information");

    if (name && !name->name.empty())
    {
        auto text = fmt::format("{} [ID: {}]", name->name, static_cast<int64_t>(selection_info.selected_entity));
        ImGui::Text("%s", text.c_str());
    }
    else if (name)
    {
        auto text = fmt::format("{} [ID: {}]", name->entity_type, static_cast<int64_t>(selection_info.selected_entity));
        ImGui::Text("%s", text.c_str());
    }
    else
    {
        auto text = fmt::format("Entity [ID: {}]", static_cast<int64_t>(selection_info.selected_entity));
        ImGui::Text("%s", text.c_str());
    }

    if (health)
    {
        ImGui::PushFont(g_header_font);
        ImGui::TextColored({.486f, .988f, 0.f, 1.f}, "%3.0f/100 HP", health->health);
        ImGui::PopFont();
    }

    if (reproduction)
    {
        ImGui::Text("I am a %s, and I have %d children.",
                    (reproduction->sex == component::Reproduction::ESex::Male ? "Male" : "Female"),
                    reproduction->number_of_children);
    }

    if (preg)
    {
        if (preg->is_egg)
        {
            ImGui::Text("I am an egg");
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
            ImGui::TableSetupColumn("Needs:");
            ImGui::TableSetupColumn("Status:");

            /** Non-clickable headers */
            cs_auto_table_headers();

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

    if (goal)
    {
        if (ImGui::BeginTable("Entity Goals", 2))
        {
            ImGui::TableSetupColumn("Goals:");
            ImGui::TableSetupColumn("Weight:");

            /** Non-clickable headers */
            cs_auto_table_headers();

            for (const auto& goal_t : goal->goals)
            {
                ImGui::TableNextCell();
                ImGui::Text("%s", goal_t.name.c_str());
                ImGui::TableNextCell();
                if (goal_t.weight_function.index() == 0)
                {
                    ImGui::Text("%3.1f", std::get<sol::function>(goal_t.weight_function)(goal_t).get<float>());
                }
                else
                {
                    ImGui::Text("%3.1f", std::get<std::function<float()>>(goal_t.weight_function)());
                }
            }
            ImGui::EndTable();
        }
    }

    if (action)
    {
        if (action->current_action_sequence.name.empty())
        {
            ImGui::Text("Current Action Sequence: %s", action->current_action_sequence.name.c_str());
            if (action->current_action_sequence.current_action.name.empty())
            {
                int action_index = 0;
                for (int i = action->current_action_sequence.actions.size() - 1; i >= 0; i--)
                {
                    if (action->current_action_sequence.actions[i] == action->current_action_sequence.current_action)
                    {
                        action_index = i;
                    }
                }
                ImGui::Text("Current Action: %s (Action %d out of %zu)",
                            action->current_action_sequence.current_action.name.c_str(),
                            action_index + 1,
                            action->current_action_sequence.actions.size());
            }
        }
    }

    if (traits && !traits->acquired_traits.empty())
    {
        if (ImGui::BeginTable("Traits", 1))
        {
            ImGui::TableSetupColumn("Traits:");
            cs_auto_table_headers();
            for (auto&& i : traits->acquired_traits)
            {
                ImGui::TableNextCell();
                ImGui::Text("%s", fmt::format("{}", i.name).c_str());
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("%s", fmt::format("{}", i.desc).c_str());
                }
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

    if (relship)
    {
        const auto parents =
            m_context->lua_state["cultsim"]["get_parents"](selection_info.selected_entity).get<system::ParentsName>();

        if (parents.mom.ids.relationship != entt::null)
        {
            ImGui::Text("My Mom is %s (%u)", parents.mom.name.c_str(), static_cast<uint32_t>(parents.mom.ids.global));
        }
        else
        {
            ImGui::Text("My Mom is the Simulation");
        }

        if (parents.dad.ids.relationship != entt::null)
        {
            ImGui::Text("My Dad is %s (%u)", parents.dad.name.c_str(), static_cast<uint32_t>(parents.dad.ids.global));
        }

        else
        {
            ImGui::Text("My Dad is the Simulation");
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
}

void ScenarioScene::handle_preference_changed(const Preference& before, const Preference& after)
{
    if (after.name == "Resolution")
    {
        m_resolution = std::get<glm::ivec2>(after.value);
    }
}
} // namespace cs
