#include "scenario_scene.h"
#include "common_helpers.h"
#include "entity/actions/action.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/tags.h"
#include "entity/memories/resource_location.h"
#include "entity/systems/action.h"
#include "entity/systems/ai.h"
#include "entity/systems/health.h"
#include "entity/systems/mitigation.h"
#include "entity/systems/movement.h"
#include "entity/systems/need.h"
#include "entity/systems/rendering.h"
#include "entity/systems/reproduction.h"
#include "entity/systems/requirement.h"
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

#include "gfx/ImGUI/imgui.h"
#include "spdlog/spdlog.h"

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

void ScenarioScene::on_enter()
{
    /** Run all initialization functions from Lua and required once for this scenario */
    m_context->lua_state["random"] = &m_rng;
    m_scenario                     = lua::quick_load_scenario(m_context->lua_state, m_scenario.script_path);
    gfx::get_renderer().set_camera_bounds(m_scenario.bounds);
    gfx::get_renderer().set_camera_position({0.f, 0.f, 1.f});
    m_scenario.init();

    /** Set up context variables in EnTT */
    m_registry.set<EntitySelectionHelper>();

    m_resolution = std::get<glm::ivec2>(m_context->preferences->get_resolution().value);
    m_context->preferences->on_preference_changed.connect<&ScenarioScene::handle_preference_changed>(this);

    /** Select entity on click */
    input::get_input()
        .fast_bind_btn(input::EKeyContext::ScenarioScene, input::EMouse::Left, input::EAction::SelectEntity, [this] {
            auto&& select_helper = m_registry.ctx<EntitySelectionHelper>();

            if (m_registry.valid(select_helper.selected_entity))
            {
                m_registry.remove<entt::tag<"selected"_hs>>(select_helper.selected_entity);
                m_registry.get<component::Sprite>(select_helper.selected_entity).texture.flag_selected = 0;
            }

            select_helper.selected_entity = select_helper.hovered_entity;

            if (m_registry.valid(select_helper.selected_entity))
            {
                m_registry.assign<entt::tag<"selected"_hs>>(select_helper.selected_entity);
                m_registry.get<component::Sprite>(select_helper.selected_entity).texture.flag_selected = 1;
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

    input::get_input().bind_action(input::EKeyContext::ScenarioScene, input::EAction::Pause, [this] {
        m_context->scene_manager->push<PauseMenuScene>();
    });
    input::get_input().add_context(input::EKeyContext::ScenarioScene);

    ai::Need need_hunger       = {static_cast<std::string>("Hunger"), 3.f, 100.f, 1.f, 0.5f, TAG_Food};
    ai::Need need_thirst       = {static_cast<std::string>("Thirst"), 4.f, 100.f, 1.5f, 1.f, TAG_Drink};
    ai::Need need_sleep        = {static_cast<std::string>("Sleep"), 1.f, 55.f, 0.5f, 0.1f, TAG_Sleep};
    ai::Need need_reproduction = {static_cast<std::string>("Reproduce"), 1.f, 100.f, 0.5f, 0.f, ETag(TAG_Reproduce | TAG_Human)};

    action::Action action_pickup_food{static_cast<std::string>("Gather food"),
                                      TAG_Find,
                                      1.f,
                                      0.f,
                                      {},
                                      [](entt::entity e, entt::entity n, entt::registry& r) {
                                          r.remove<component::Position>(n);
                                          r.remove<component::Sprite>(n);
                                          r.get<component::Inventory>(e).contents.push_back(n);
                                      },
                                      [](entt::entity e, entt::registry& r) {
                                          spdlog::get("agent")->debug("Agent {} failed to finish action: gather food", e);
                                          r.destroy(e);
                                      },
                                      {}};

    action::Action action_eat{
        static_cast<std::string>("Eat"),
        TAG_Find,
        5.f,
        0.f,
        {},
        [](entt::entity e, entt::entity n, entt::registry& r) {
            r.destroy(n);
            for (auto& need : r.get<component::Needs>(e).needs)
            {
                if (need.tags & TAG_Food)
                {
                    need.status += 80.f;
                }
            }

            if (auto memories = r.try_get<component::Memory>(e); memories)
            {
                for (auto& container : memories->memory_container)
                {
                    if (container.memory_tags & TAG_Food && container.memory_tags & TAG_Location)
                    {
                        auto vision = r.get<component::Vision>(e);
                        int count{};
                        auto location  = r.get<component::Position>(e).position;
                        bool duplicate = false;
                        for (auto entity : vision.seen)
                        {
                            if (r.valid(entity))
                            {
                                auto entity_tags = r.try_get<component::Tags>(entity);
                                if (entity_tags && entity_tags->tags & TAG_Food)
                                {
                                    count++;
                                }
                            }
                        }
                        for (auto& memory : container.memory_storage)
                        {
                            if (auto* res = dynamic_cast<memory::ResourceLocation*>(memory.get());
                                res && close_enough(res->m_location, location, 25.f))
                            {
                                res->m_number_of_entities = count;
                                duplicate                 = true;
                            }
                        }
                        if (!duplicate)
                        {
                            container.memory_storage.push_back(std::unique_ptr<memory::ResourceLocation>(
                                new memory::ResourceLocation(ETag(TAG_Food | TAG_Location), location, count)));
                        }
                    }
                }
            }
        },
        [](entt::entity e, entt::registry& r) {
            spdlog::get("agent")->warn("We failed to finish action: eat");
            r.destroy(e);
        },
        []() {
            spdlog::get("agent")->warn("We aborted our action");
        }};

    action::Action action_inventory_food{static_cast<std::string>("Check inventory for food"),
                                         {},
                                         2.0f,
                                         0.f,
                                         {},
                                         [](entt::entity e, entt::entity n, entt::registry& r) {
                                             auto inventory = r.try_get<component::Inventory>(e);
                                             if (inventory && inventory->tags & TAG_Food)
                                             {
                                                 int i = 0;
                                                 for (auto& content : inventory->contents)
                                                 {
                                                     if (r.get<component::Tags>(content).tags & TAG_Food)
                                                     {
                                                         for (auto& need : r.get<component::Needs>(e).needs)
                                                         {
                                                             if (need.tags | TAG_Food)
                                                             {
                                                                 need.status += 80.f;
                                                             }
                                                         }
                                                         inventory->contents.erase(inventory->contents.begin() + i);
                                                         return;
                                                     }
                                                     i++;
                                                 }
                                             }
                                         },
                                         [](entt::entity e, entt::registry& r) {
                                             auto inventory = r.try_get<component::Inventory>(e);
                                             if (inventory && inventory->tags & TAG_Food)
                                             {
                                                 int i = 0;
                                                 for (auto& content : inventory->contents)
                                                 {
                                                     if (r.get<component::Tags>(content).tags & TAG_Food)
                                                     {
                                                         inventory->contents.erase(inventory->contents.begin() + i);
                                                         return;
                                                     }
                                                     i++;
                                                 }
                                             }
                                         },
                                         {}};
    action::Action action_drink{
        static_cast<std::string>("Drink"),
        TAG_Find,
        2.f,
        0.f,
        {},
        [](entt::entity e, entt::entity n, entt::registry& r) {
            r.destroy(n);
            for (auto& need : r.get<component::Needs>(e).needs)
            {
                if (need.tags & TAG_Drink)
                {
                    need.status += 80.f;
                }
                if (auto memories = r.try_get<component::Memory>(e); memories)
                {
                    for (auto& container : memories->memory_container)
                    {
                        if (container.memory_tags & TAG_Drink && container.memory_tags & TAG_Location)
                        {
                            std::time_t timestamp = std::time(0);
                            auto vision           = r.get<component::Vision>(e);
                            int count{};
                            auto location  = r.get<component::Position>(e).position;
                            bool duplicate = false;
                            for (auto entity : vision.seen)
                            {
                                if (r.valid(entity))
                                {
                                    auto entity_tags = r.try_get<component::Tags>(entity);
                                    if (entity_tags && entity_tags->tags & TAG_Drink)
                                    {
                                        count++;
                                    }
                                }
                            }
                            for (auto& memory : container.memory_storage)
                            {
                                if (auto* res = dynamic_cast<memory::ResourceLocation*>(memory.get());
                                    res && close_enough(res->m_location, location, 50.f))
                                {
                                    res->m_number_of_entities = count;
                                    duplicate                 = true;
                                }
                            }
                            if (!duplicate)
                            {
                                container.memory_storage.push_back(std::unique_ptr<memory::ResourceLocation>(
                                    new memory::ResourceLocation(ETag(TAG_Drink | TAG_Location), location, count)));
                            }
                        }
                    }
                }
            }
        },
        [](entt::entity e, entt::registry& r) {
            spdlog::get("agent")->warn("We failed to finish action: drink");
            r.destroy(e);
        },
        []() {
            spdlog::get("agent")->warn("We aborted our action");
        }};

    action::Action action_sleep{
        static_cast<std::string>("Sleep"),
        TAG_None,
        10.f,
        0.f,
        {},
        [](entt::entity e, entt::entity n, entt::registry& r) {
            for (auto& need : r.get<component::Needs>(e).needs)
            {
                if (need.tags & TAG_Sleep)
                {
                    need.status += 69.f;
                }
            }
        },
        [](entt::entity e, entt::registry& r) { spdlog::get("agent")->warn("Agent {} failed to finish action: sleep", e); },
        []() {
            spdlog::get("agent")->warn("We aborted our action");
        }};

    action::Action action_reproduce{
        static_cast<std::string>("Reproduce"),
        ETag(TAG_Find | TAG_Tag),
        5.f,
        0.f,
        {},
        [](entt::entity e, entt::entity n, entt::registry& r) {
            if (r.valid(e) && r.valid(n))
            {
                auto repr = r.try_get<component::Reproduction>(e);

                auto target_repr = r.try_get<component::Reproduction>(n);

                if (repr != nullptr)
                {
                    auto needs = r.try_get<component::Needs>(e);
                    if (needs != nullptr)
                    {
                        for (auto& need : needs->needs)
                        {
                            if ((need.tags & TAG_Reproduce) && need.status < 50.f)
                            {
                                need.status += 100.f;
                            }
                        }
                    }

                    if (target_repr != nullptr)
                    {
                        if (repr->sex == component::Reproduction::Female && target_repr->sex == component::Reproduction::Male)
                        {
                            r.assign<component::Timer>(e, 20.f, 0.f, 1, [repr, target_repr](entt::entity e, entt::registry& r) {
                                if (r.valid(e))
                                {
                                    auto child         = r.create(e, r);
                                    auto& child_needs  = r.get<component::Needs>(child);
                                    auto& child_reprd  = r.get<component::Reproduction>(child);
                                    auto& child_health = r.get<component::Health>(child);
                                    auto& child_pos    = r.get<component::Position>(child);
                                    auto& child_strat  = r.get<component::Strategies>(child);
                                    for (auto& strat : child_strat.strategies)
                                    {
                                        for (auto& action : strat.actions)
                                        {
                                            if (action.target != entt::null)
                                            {
                                                action.target = child;
                                            }
                                            if (action.target != child)
                                            {
                                                spdlog::get("agent")->error("child: {}, action.target: {}", child, action.target);
                                            }
                                        }
                                    }
                                    for (auto need : child_needs.needs)
                                    {
                                        need.status = 100.f;
                                    }

                                    RandomEngine rng{};
                                    child_reprd.number_of_children = 0;
                                    child_health.hp                = 100.f;
                                    child_pos.position             = r.get<component::Position>(e).position +
                                                         glm::vec3(rng.uniform(-10.f, 10.f), rng.uniform(-10.f, 10.f), 0.f);

                                    if (rng.trigger(0.5f))
                                    {
                                        child_reprd.sex = component::Reproduction::Male;
                                    }
                                    repr->number_of_children++;
                                    target_repr->number_of_children++;
                                }
                            });
                        }

                        auto& target_needs = r.get<component::Needs>(n);
                        auto& target_tags  = r.get<component::Tags>(n);
                        for (auto& need : target_needs.needs)
                        {
                            if ((need.tags & TAG_Reproduce) && need.status < 50.f)
                            {
                                need.status += 100.f;
                            }
                        }
                    }
                }
            }
        },
        [](entt::entity e, entt::registry& r) { spdlog::get("agent")->warn("We failed to finish action: reproduce"); },
        []() {
            spdlog::get("agent")->warn("We aborted our action: reproduce");
        }};

    ai::Strategy strategy_findfood  = {static_cast<std::string>("Looking for Food"),
                                      0,
                                      {},
                                      TAG_Food,
                                      std::vector<action::Action>{action_eat, action_inventory_food}};
    ai::Strategy strategy_finddrink = {static_cast<std::string>("Looking for Water"),
                                       0,
                                       {},
                                       TAG_Drink,
                                       std::vector<action::Action>{action_drink}};
    ai::Strategy strategy_sleep     = {static_cast<std::string>("Sleep"),
                                   0,
                                   {},
                                   TAG_Sleep,
                                   std::vector<action::Action>{action_sleep}};

    ai::Strategy strategy_breed = {static_cast<std::string>("Looking for a mate"),
                                   0,
                                   {},
                                   ETag(TAG_Reproduce | TAG_Human),
                                   std::vector<action::Action>{action_reproduce}};

    ai::Strategy gather_food = {static_cast<std::string>("Gathering Food"),
                                0,
                                {},
                                ETag(TAG_Food | TAG_Gather),
                                std::vector<action::Action>{action_pickup_food}};

    auto tex   = gfx::get_renderer().sprite().get_texture("sprites/agent_c.png");
    auto f_tex = gfx::get_renderer().sprite().get_texture("sprites/food_c.png");
    auto d_tex = gfx::get_renderer().sprite().get_texture("sprites/liquid_c.png");
    auto t_tex = gfx::get_renderer().sprite().get_texture("sprites/circle.png");

    for (int i = 1; i <= m_scenario.agent_count; i++)
    {
        auto agent = m_registry.create();
        int i1     = i;
        if (i % 2 == 1)
        {
            i1 = -i;
        }
        strategy_sleep.actions.front().target = agent;
        glm::vec2 pos(m_rng.uniform(-m_scenario.bounds.x, m_scenario.bounds.x),
                      m_rng.uniform(-m_scenario.bounds.y, m_scenario.bounds.y));
        m_registry.assign<component::Position>(agent, glm::vec3(pos, 0));
        m_registry.assign<component::Movement>(agent, std::vector<glm::vec3>{}, glm::vec2{}, 80.f, 0.f);
        m_registry.assign<component::Sprite>(agent, tex, glm::vec3(1.f, 0.f, 0.f));
        m_registry.assign<component::Vision>(agent, std::vector<entt::entity>{}, 40.f, static_cast<uint8_t>(0));
        m_registry.assign<component::Tags>(agent, ETag(TAG_Avoidable));
        m_registry.assign<component::Needs>(agent,
                                            std::vector<ai::Need>{need_hunger, need_thirst, need_sleep, need_reproduction},
                                            std::vector<ai::Need>{});
        cs::component::Reproduction::ESex gender = cs::component::Reproduction::Male;
        if (i % 2 == 1)
        {
            gender = cs::component::Reproduction::Female;
        }
        m_registry.assign<component::Reproduction>(agent, gender, uint16_t(0), uint16_t(5));
        m_registry.assign<component::Strategies>(
            agent,
            std::vector<ai::Strategy>({strategy_findfood, strategy_finddrink, strategy_sleep, strategy_breed}),
            std::vector<ai::Strategy>{});
        m_registry.assign<component::Health>(agent, 100.f, 1.f, ETag(TAG_Food | TAG_Drink | TAG_Sleep));
        auto& memory_comp = m_registry.assign<component::Memory>(agent, std::vector<memory::Container>{});
        memory_comp.memory_container.emplace_back(ETag(TAG_Food | TAG_Location));
        memory_comp.memory_container.emplace_back(ETag(TAG_Drink | TAG_Location));
    }

    for (int j = 0; j < 75; j++)
    {
        auto trees = m_registry.create();
        m_registry.assign<component::Position>(trees,
                                               glm::vec3(m_rng.uniform(-m_scenario.bounds.x + m_scenario.bounds.x / 20.f,
                                                                       m_scenario.bounds.x - m_scenario.bounds.x / 20.f),
                                                         m_rng.uniform(-m_scenario.bounds.y + m_scenario.bounds.y / 20.f,
                                                                       m_scenario.bounds.y - m_scenario.bounds.x / 20.f),
                                                         0.f));
        m_registry.assign<component::Sprite>(trees, t_tex, glm::vec3(0.8f, 0.5f, 0.1f));
        m_registry.assign<component::Tags>(trees, TAG_Avoidable);
        m_registry
            .assign<component::Timer>(trees, m_rng.uniform(50.f, 140.f), 0.f, -1, [f_tex](entt::entity e, entt::registry& r) {
                auto food = r.create();
                auto pos  = r.get<component::Position>(e).position;
                static RandomEngine rng;
                r.assign<component::Position>(food,
                                              glm::vec3(pos.x + rng.uniform(-10.f, 10.f), pos.y + rng.uniform(-10.f, 10.f), 0.f));
                r.assign<component::Sprite>(food, f_tex, glm::vec3(0.9f, 0.6f, 0.1f));
                r.assign<component::Tags>(food, TAG_Food);
            });
    }

    for (int k = 0; k < 75; k++)
    {
        auto ponds = m_registry.create();
        m_registry.assign<component::Position>(ponds,
                                               glm::vec3(m_rng.uniform(-m_scenario.bounds.x + m_scenario.bounds.x / 20.f,
                                                                       m_scenario.bounds.x - m_scenario.bounds.x / 20.f),
                                                         m_rng.uniform(-m_scenario.bounds.y + m_scenario.bounds.y / 20.f,
                                                                       m_scenario.bounds.y - m_scenario.bounds.x / 20.f),
                                                         0.f));
        m_registry.assign<component::Sprite>(ponds, t_tex, glm::vec3(0.1f, 0.2f, 0.6f));
        m_registry.assign<component::Tags>(ponds, TAG_Avoidable);
        m_registry
            .assign<component::Timer>(ponds, m_rng.uniform(25.f, 120.f), 0.f, -1, [d_tex](entt::entity e, entt::registry& r) {
                auto drink = r.create();
                auto pos   = r.get<component::Position>(e).position;
                static RandomEngine rng;
                r.assign<component::Position>(drink,
                                              glm::vec3(pos.x + rng.uniform(-10.f, 10.f), pos.y + rng.uniform(-10.f, 10.f), 0.f));
                r.assign<component::Sprite>(drink, d_tex, glm::vec3(0.1f, 0.7f, 1.f));
                r.assign<component::Tags>(drink, TAG_Drink);
            });
    }

    /** Add systems specified by scenario */
    for (const auto& system : m_scenario.systems)
    {
        auto type = entt::resolve(entt::hashed_string(system.c_str()));
        if (type)
        {
            m_active_systems.emplace_back(type.construct(system::SystemContext{&m_registry, &m_dispatcher, &m_rng, &m_scenario}));
        }
        else
        {
            spdlog::get("scenario")->warn("adding system \"{}\" that is unknown", system);
        }
    }

    //    /** Update systems */
    //    for (auto&& system : m_active_systems)
    //    {
    //        system.type().func("update"_hs).invoke(system, 20.f);
    //    }
}

void ScenarioScene::on_exit()
{
    input::get_input().remove_context(input::EKeyContext::ScenarioScene);
    m_context->preferences->on_preference_changed.disconnect<&ScenarioScene::handle_preference_changed>(this);
}

bool ScenarioScene::update(float dt)
{
    dt *= m_timescale;

    m_simtime += dt;
    m_next_data_sample += dt;

    // TODO : Move to input action response
    update_entity_hover();

    setup_docking_ui();
    ImGui::Begin(m_scenario.name.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar);
    draw_scenario_information_ui();
    draw_time_control_ui();
    draw_selected_entity_information_ui();

    static auto b_tex = gfx::get_renderer().sprite().get_texture("sprites/background_c.png");
    b_tex.scale       = 100;

    /** Draw background crudely */
    for (int i = -m_scenario.bounds.x / 100; i <= m_scenario.bounds.x / 100; i++)
    {
        for (int j = -m_scenario.bounds.y / 100; j <= m_scenario.bounds.y / 100; j++)
        {
            gfx::get_renderer().sprite().draw(glm::vec3(i * 100.f, j * 100.f, 0.f), glm::vec3(0.05f, 0.17f, 0.1f), b_tex);
        }
    }

    /** Update systems */
    for (auto&& system : m_active_systems)
    {
        system.type().func("update"_hs).invoke(system, dt);
    }

    /** Deal with long running tasks, then events */
    m_scheduler.update(dt);
    m_dispatcher.update();
    m_scenario.update(dt);

    ImGui::End();

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

    r_debug.draw_line({-100.f, 0.f, 0.f}, {100.f, 0.f, 0.f}, {1.f, 0.f, 0.f});
    r_debug.draw_line({0.f, -100.f, 0.f}, {0.f, 100.f, 0.f}, {0.f, 1.f, 0.f});
    r_debug.draw_line({0.f, 0.f, -100.f}, {0.f, 0.f, 100.f}, {0.f, 0.f, 1.f});

    r_debug.draw_line({-m_scenario.bounds.x, -m_scenario.bounds.y, 0.f},
                      {m_scenario.bounds.x, -m_scenario.bounds.y, 0.f},
                      {0.f, 1.f, 0.f});
    r_debug.draw_line({-m_scenario.bounds.x, -m_scenario.bounds.y, 0.f},
                      {-m_scenario.bounds.x, m_scenario.bounds.y, 0.f},
                      {0.f, 1.f, 0.f});
    r_debug.draw_line({m_scenario.bounds.x, m_scenario.bounds.y, 0.f},
                      {-m_scenario.bounds.x, m_scenario.bounds.y, 0.f},
                      {0.f, 1.f, 0.f});
    r_debug.draw_line({m_scenario.bounds.x, -m_scenario.bounds.y, 0.f},
                      {m_scenario.bounds.x, m_scenario.bounds.y, 0.f},
                      {0.f, 1.f, 0.f});

    m_scenario.draw();
    return false;
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
    // TODO : Get rid of after prototype
    static std::vector<float> living_entities{};

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

    /** Entity count graph */
    if (m_next_data_sample > m_scenario.sampling_rate)
    {
        m_next_data_sample = 0.f;
        living_entities.push_back(m_registry.size<component::Needs>());
    }

    /** Plot number of living entities */
    ImGui::PlotLines("##Alive", living_entities.data(), living_entities.size(), 0, "Living Agents", FLT_MAX, FLT_MAX, {0, 75});
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
    if (ImGui::Button(">", {36, 24}))
    {
        m_timescale = 1.f;
    }
    ImGui::SameLine();
    if (ImGui::Button(">>", {36, 24}))
    {
        m_timescale = 2.5f;
    }
    ImGui::SameLine();
    if (ImGui::Button(">>>", {36, 24}))
    {
        m_timescale = 5.f;
    }
    ImGui::SameLine();
    if (ImGui::Button(">>>>", {36, 24}))
    {
        m_timescale = 10.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("Turbo", {36, 24}))
        if (ImGui::Button("!!", {24, 24}))
        {
            m_timescale = 25.f;
        }
    ImGui::SameLine();
    if (ImGui::Button("!!!", {24, 24}))
    {
        m_timescale = 100.f;
    }
    ImGui::End();

    ImGui::End();
}

void ScenarioScene::draw_selected_entity_information_ui()
{
    const auto& selection_info = m_registry.ctx<EntitySelectionHelper>();
    if (!m_registry.valid(selection_info.selected_entity))
    {
        return;
    }

    const auto& [needs, health, strategy] =
        m_registry.try_get<component::Needs, component::Health, component::Strategies>(selection_info.selected_entity);

    ImGui::SetNextWindowPos({250.f, 250.f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({400.f, 600.f}, ImGuiCond_FirstUseEver);
    ImGui::Begin("Agent Information");

    auto text = fmt::format("Ola Normann nr {}", static_cast<int64_t>(selection_info.selected_entity));
    ImGui::Text(text.c_str());

    if (health)
    {
        ImGui::PushFont(g_header_font);
        ImGui::TextColored({.486f, .988f, 0.f, 1.f}, "%3.0f/100 HP", health->hp);
        ImGui::PopFont();
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
        hover_view.get<component::Sprite>(selection_helper.hovered_entity).texture.flag_hovered = 0;
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
        hover_view.get<component::Sprite>(selection_helper.hovered_entity).texture.flag_hovered = 1;
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
