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
            auto meta = type.construct(system::SystemContext{&m_registry, &m_dispatcher, &m_rng, &m_scenario, &m_mt_executor});
            system::ISystem& temp_ref = meta.cast<system::ISystem>();
            m_active_systems.emplace_back(temp_ref.clone());
            m_active_systems.back()->initialize();
        }
        else
        {
            spdlog::get("scenario")->warn("adding system \"{}\" that is unknown", system);
        }
    }
}

void ScenarioScene::clean_simulation()
{
    m_data_collector.save_to_file(m_scenario.name + "_data", true);

    m_registry.clear();
    m_simtime   = 0.f;
    m_timescale = 1.f;
    m_data_collector.clear();

    /** Deinitialize systems and then clear them */
    for (auto& system : m_active_systems)
    {
        system->deinitialize();
    }
    m_active_systems.clear();

    for (auto& system : m_inactive_systems)
    {
        system->initialize();
    }
    m_inactive_systems.clear();
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
    dt *= m_timescale;
    m_simtime += dt;

    // TODO : Move to input action response
    update_entity_hover();

    setup_docking_ui();
    ImGui::Begin(m_scenario.name.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar);
    draw_scenario_information_ui();
    draw_time_control_ui();
    draw_selected_entity_information_ui();

    /** Sample data */
    m_data_collector.update(dt);
    m_data_collector.show_ui();

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

    /** Update systems */
    for (auto&& system : m_active_systems)
    {
        system->update(dt);
    }

    /** Deal with long running tasks, then events */
    m_scheduler.update(dt);
    m_dispatcher.update();
    m_scenario.update(dt);

    /** It's supposed to be two of these here, do not change - not a bug */
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
    ImGui::Separator();
    ImGui::Text("GRID");
    static bool show_grid = false;
    static int grid_span  = 25;
    static int grid_size  = 32;
    ImGui::Checkbox("Show Grid", &show_grid);
    ImGui::DragInt("Grid Span", &grid_span, 1.f, 0, 50);
    ImGui::DragInt("Grid Size", &grid_size, 1.f, 0, 256);

    auto& r_debug = gfx::get_renderer().debug();
    if (show_grid)
    {
        for (int i = -grid_span; i <= grid_span; i++)
        {
            r_debug.draw_line(glm::vec3(-grid_size * grid_span, i * grid_size, 0),
                              glm::vec3(grid_size * grid_span, i * grid_size, 0),
                              glm::vec3(0.3f));

            r_debug.draw_line(glm::vec3(i * grid_size, -grid_size * grid_span, 0),
                              glm::vec3(i * grid_size, grid_size * grid_span, 0),
                              glm::vec3(0.3f));
        }
    }
    static bool show_axis = false;
    ImGui::Checkbox("Show Axis", &show_axis);
    if (show_axis)
    {
        r_debug.draw_line({-100.f, 0.f, 0.f}, {100.f, 0.f, 0.f}, {1.f, 0.f, 0.f});
        r_debug.draw_line({0.f, -100.f, 0.f}, {0.f, 100.f, 0.f}, {0.f, 1.f, 0.f});
        r_debug.draw_line({0.f, 0.f, -100.f}, {0.f, 0.f, 100.f}, {0.f, 0.f, 1.f});
    }

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
        m_timescale = std::clamp(m_timescale *= 2, 0.05f, 100.f);
    });

    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::SpeedDown, [this]() {
        m_timescale = std::clamp(m_timescale /= 2, 0.05f, 100.f);
    });

    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::Pause, [this]() { m_timescale = 0; });

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

    /** Get component from Lua */
    sol::table cultsim = lua.create_table("cultsim");
    cultsim.set_function("get_component", [this](sol::this_state s, entt::entity e, uint32_t id) -> sol::object {
        switch (id)
        {
            case entt::type_info<component::Position>::id():
                return sol::make_object(s, &m_registry.get<component::Position>(e));
                break;
            case entt::type_info<component::Movement>::id():
                return sol::make_object(s, &m_registry.get<component::Movement>(e));
                break;
            case entt::type_info<component::Sprite>::id():
                return sol::make_object(s, &m_registry.get<component::Sprite>(e));
                break;
            case entt::type_info<component::Vision>::id():
                return sol::make_object(s, &m_registry.get<component::Vision>(e));
                break;
            case entt::type_info<component::Tags>::id(): return sol::make_object(s, &m_registry.get<component::Tags>(e)); break;
            case entt::type_info<component::Need>::id(): return sol::make_object(s, &m_registry.get<component::Need>(e)); break;
            case entt::type_info<component::Reproduction>::id():
                return sol::make_object(s, &m_registry.get<component::Reproduction>(e));
                break;
            case entt::type_info<component::Strategy>::id():
                return sol::make_object(s, &m_registry.get<component::Strategy>(e));
                break;
            case entt::type_info<component::Health>::id():
                return sol::make_object(s, &m_registry.get<component::Health>(e));
                break;
            case entt::type_info<component::Memory>::id():
                return sol::make_object(s, &m_registry.get<component::Memory>(e));
                break;
            case entt::type_info<component::Attack>::id():
                return sol::make_object(s, &m_registry.get<component::Attack>(e));
                break;
            default:
                spdlog::critical("Can not find the componet you are looking for");
                return sol::nil;
                break;
        }
    });

    /** Helper action to modify an entity need */
    cultsim.set_function("modify_need", [this](sol::this_state s, entt::entity e, ETag need_tags, float delta) {
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

    /** Apply Damage */
    cultsim.set_function("apply_basic_damage", [this](sol::this_state s, entt::entity e, float damage) {
        if (auto* health = m_registry.try_get<component::Health>(e); health)
        {
            health->health -= damage;
        }
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
    cultsim.set_function("kill", [this](sol::this_state s, entt::entity e) {
        if (e == entt::null)
        {
            spdlog::get("agent")->critical("Trying to kill a null agent");
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
        m_registry
            .assign_or_replace<component::Timer>(mother, 20.f, 0.f, 1, [this, father, mother](entt::entity e, entt::registry& r) {
                /** Don't do anything if e became invalid */
                if (!r.valid(e))
                {
                    return;
                }

                /** Figure out "type" of mother and spawn a child based on that */
                const auto& mother_meta = r.get<component::Meta>(e);

                /** Set position to be close to mother */
                const auto new_pos = r.get<component::Position>(e).position +
                                     glm::vec3(m_rng.uniform(-10.f, 10.f), m_rng.uniform(-10.f, 10.f), 0.f);

                auto child = spawn_entity(m_registry, m_context->lua_state, mother_meta.name, new_pos);

                /** Reset all needs to 100.0 */
                if (auto* need_comp = r.try_get<component::Need>(child); need_comp)
                {
                    for (auto& need : need_comp->needs)
                    {
                        need.status = 100.f;
                    }
                }

                /** Reset health */
                if (auto* health_comp = r.try_get<component::Health>(child); health_comp)
                {
                    health_comp->health = 100.f;
                }

                /** Give a child to the parents */
                if (m_registry.valid(father))
                {
                    if (auto* rc = m_registry.try_get<component::Reproduction>(father); rc)
                    {
                        ++rc->number_of_children;
                    }
                }

                if (auto* rc = m_registry.try_get<component::Reproduction>(mother); rc)
                {
                    ++rc->number_of_children;
                }
            });
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
    ImGui::TextColored({1., 0.627, 0.478, 1.}, "Runtime: %4.1f (%2.1fx)", m_simtime, m_timescale);
    ImGui::Spacing();
    ImGui::PopFont();
    ImGui::Separator();
}

void ScenarioScene::draw_time_control_ui()
{
    ImGui::SetNextWindowPos({m_resolution.x / 2.f, 0.f}, 0, {0.5f, 0.f});
    ImGui::SetNextWindowSize({360, 64});
    ImGui::Begin("Time Control", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::Text("Time Scaling");
    if (ImGui::Button("||", {36, 24}))
    {
        m_timescale = 0.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("1x", {36, 24}))
    {
        m_timescale = 1.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("2.5x", {36, 24}))
    {
        m_timescale = 2.5f;
    }
    ImGui::SameLine();
    if (ImGui::Button("5x", {36, 24}))
    {
        m_timescale = 5.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("10x", {36, 24}))
    {
        m_timescale = 10.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("25x", {36, 24}))
    {
        m_timescale = 25.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("100x", {36, 24}))
    {
        m_timescale = 100.f;
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

    const auto& [needs, health, strategy, reproduction, timer, tags, memories] =
        m_registry.try_get<component::Need,
                           component::Health,
                           component::Strategy,
                           component::Reproduction,
                           component::Timer,
                           component::Tags,
                           component::Memory>(selection_info.selected_entity);

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
        ImGui::Text("I am a %s, and I have %d children.",
                    (reproduction->sex == component::Reproduction::ESex::Male ? "Male" : "Female"),
                    reproduction->number_of_children);
    }

    if (strategy)
    {
        if (!strategy->staged_strategies.empty())
        {
            ImGui::Text("Currently: %s", strategy->staged_strategies.front().name.c_str());
            if (!strategy->staged_strategies.front().actions.empty())
            {
                const auto& action = strategy->staged_strategies.front().actions.front();
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
