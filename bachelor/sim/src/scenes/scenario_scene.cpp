#include "scenario_scene.h"
#include "entity/actions/action.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/tags.h"
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

#include <common_helpers.h>
#include <functional>
#include <memory>
#include <random>

#include "gfx/ImGUI/imgui.h"
#include "spdlog/spdlog.h"

namespace cs
{
ScenarioScene::ScenarioScene(lua::Scenario scenario) : m_scenario(std::move(scenario))
{
}

void ScenarioScene::on_enter()
{
    m_scenario.init();
    m_context->lua_state["random"] = &m_rng;
    m_context->lua_state.script(fs::read_file("script/needs.lua"));
    gfx::get_renderer().set_camera_bounds(m_scenario.bounds);
    gfx::get_renderer().set_camera_position({0.f, 0.f, 1.f});

    input::get_input().fast_bind_key(input::KeyContext::ScenarioScene, SDL_SCANCODE_P, input::Action::Pause, [this] {
        m_context->scene_manager->push<PauseMenuScene>();
    });
    input::get_input().add_context(input::KeyContext::ScenarioScene);

    ai::Need need_hunger       = {static_cast<std::string>("hunger"), 3.f, m_rng.uniform(26.f, 76.f), 1.f, TAG_Food};
    ai::Need need_thirst       = {static_cast<std::string>("thirst"), 4.f, m_rng.uniform(48.f, 86.f), 1.5f, TAG_Drink};
    ai::Need need_sleep        = {static_cast<std::string>("sleep"), 1.f, 96.f, 0.5f, TAG_Sleep};
    ai::Need need_reproduction = {static_cast<std::string>("reproduce"), 1.f, 100.f, 0.5f, ETag(TAG_Reproduce | TAG_Human)};

    action::Action action_eat{static_cast<std::string>("eat"),
                              TAG_Find,
                              5.f,
                              0.f,
                              {},
                              [](entt::entity e, entt::entity n, entt::registry& r) {
                                  spdlog::warn("We finished action: eat on entity: {}", e);
                                  r.destroy(n);
                                  for (auto& need : r.get<component::Needs>(e).needs)
                                  {
                                      if (need.tags & TAG_Food)
                                      {
                                          spdlog::warn("Current status of need FOOD: {}", need.status);
                                          need.status += 80.f;
                                      }
                                  }
                              },
                              [](entt::entity e, entt::registry& r) {
                                  spdlog::warn("We failed to finish action: eat");
                                  r.destroy(e);
                              },
                              []() {
                                  spdlog::warn("We aborted our action");
                              }};

    action::Action action_drink{static_cast<std::string>("drink"),
                                TAG_Find,
                                2.f,
                                0.f,
                                {},
                                [](entt::entity e, entt::entity n, entt::registry& r) {
                                    spdlog::warn("We finished action: drink on entity: {}", e);
                                    r.destroy(n);
                                    for (auto& need : r.get<component::Needs>(e).needs)
                                    {
                                        if (need.tags & TAG_Drink)
                                        {
                                            spdlog::warn("Current status of need DRINK: {}", need.status);
                                            need.status += 80.f;
                                        }
                                    }
                                },
                                [](entt::entity e, entt::registry& r) {
                                    spdlog::warn("We failed to finish action: drink");
                                    r.destroy(e);
                                },
                                []() {
                                    spdlog::warn("We aborted our action");
                                }};

    action::Action action_sleep{static_cast<std::string>("sleep"),
                                {},
                                1.f,
                                0.f,
                                {},
                                [](entt::entity e, entt::entity n, entt::registry& r) {
                                    spdlog::warn("We finished action: sleep on entity: {}", e);
                                    for (auto& need : r.get<component::Needs>(e).needs)
                                    {
                                        if (need.tags & TAG_Sleep)
                                        {
                                            spdlog::warn("Current status of need SLEEP: {}", need.status);
                                            need.status += 10.f;
                                        }
                                    }
                                },
                                [](entt::entity e, entt::registry& r) { spdlog::warn("We failed to finish action: sleep"); },
                                []() {
                                    spdlog::warn("We aborted our action");
                                }};

    action::Action action_reproduce{
        static_cast<std::string>("Reproduce"),
        ETag(TAG_Find | TAG_Tag),
        5.f,
        0.f,
        {},
        [](entt::entity e, entt::entity n, entt::registry& r) {
            spdlog::warn("We finished action: reproduce on entity{} with entity{}", e, n);

            if (r.has<component::Reproduction>(e))
            {
                spdlog::warn("We have a reproduction component");
                auto& repr = r.get<component::Reproduction>(e);
                repr.number_of_children++;
            }

            if (r.has<component::Reproduction>(n))
            {
                spdlog::warn("Target has Reproduction component");
                auto& target_repr = r.get<component::Reproduction>(n);
            }

            if (r.has<component::Reproduction>(e) && r.has<component::Reproduction>(n))
            {
                auto repr        = r.get<component::Reproduction>(e);
                auto target_repr = r.get<component::Reproduction>(n);
                if (repr.sex == component::Reproduction::Female && target_repr.sex == component::Reproduction::Male)
                {
                    spdlog::warn("We are making a baby");
                    auto child         = r.create(e, r);
                    auto& child_needs  = r.get<component::Needs>(child);
                    auto& child_reprd  = r.get<component::Reproduction>(child);
                    auto& child_health = r.get<component::Health>(child);
                    auto& child_pos    = r.get<component::Position>(child);
                    for (auto need : child_needs.needs)
                    {
                        need.status = 100.f;
                    }
                    child_reprd.number_of_children = 0;
                    child_health.hp                = 100.f;
                    child_pos.position             = r.get<component::Position>(e).position + glm::vec3(10.f, 10.f, 0.f);
                    spdlog::warn("Child entity: {}", child);
                    RandomEngine rng{};
                    if (rng.trigger(0.5f))
                    {
                        child_reprd.sex = component::Reproduction::Male;
                    }
                }
            }
            auto& needs        = r.get<component::Needs>(e);
            auto& tags         = r.get<component::Tags>(e);
            auto& target_needs = r.get<component::Needs>(n);
            auto& target_tags  = r.get<component::Tags>(n);
            for (auto& need : needs.needs)
            {
                if ((need.tags & TAG_Reproduce) && need.status < 50.f)
                {
                    spdlog::warn("Current status of need REPRODUCE: {}", need.status);
                    need.status += 100.f;
                }
            }

            for (auto& need : target_needs.needs)
            {
                if ((need.tags & TAG_Reproduce) && need.status < 50.f)
                {
                    spdlog::warn("Current status of need REPRODUCE: {}", need.status);
                    need.status += 100.f;
                }
            }
        },
        [](entt::entity e, entt::registry& r) { spdlog::warn("We failed to finish action: reproduce"); },
        []() {
            spdlog::warn("We aborted our action: reproduce");
        }};

    ai::Strategy strategy_findfood  = {static_cast<std::string>("eat food"),
                                      0,
                                      {},
                                      TAG_Food,
                                      std::vector<action::Action>{action_eat}};
    ai::Strategy strategy_finddrink = {static_cast<std::string>("drink water"),
                                       0,
                                       {},
                                       TAG_Drink,
                                       std::vector<action::Action>{action_drink}};
    ai::Strategy strategy_sleep     = {static_cast<std::string>("sleep"),
                                   0,
                                   {},
                                   TAG_Sleep,
                                   std::vector<action::Action>{action_sleep}};

    ai::Strategy strategy_breed = {static_cast<std::string>("reproduce"),
                                   0,
                                   {},
                                   ETag(TAG_Reproduce | TAG_Human),
                                   std::vector<action::Action>{action_reproduce}};

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
        m_registry.assign<component::Reproduction>(agent, gender, uint16_t(0), uint16_t(2));
        m_registry.assign<component::Strategies>(
            agent,
            std::vector<ai::Strategy>({strategy_findfood, strategy_finddrink, strategy_sleep, strategy_breed}),
            std::vector<ai::Strategy>{});
        m_registry.assign<component::Health>(agent, 100.f, 1.f, ETag(TAG_Food | TAG_Drink | TAG_Sleep));
    }

    for (int j = 0; j < 100; j++)
    {
        auto trees = m_registry.create();
        m_registry.assign<component::Position>(trees,
                                               glm::vec3(m_rng.uniform(-m_scenario.bounds.x, m_scenario.bounds.x),
                                                         m_rng.uniform(-m_scenario.bounds.y, m_scenario.bounds.y),
                                                         0.f));
        m_registry.assign<component::Sprite>(trees, t_tex, glm::vec3(0.8f, 0.5f, 0.1f));
        m_registry.assign<component::Tags>(trees, TAG_Avoidable);
        m_registry
            .assign<component::Timer>(trees, m_rng.uniform(25.f, 100.f), 0.f, -1, [f_tex](entt::entity e, entt::registry& r) {
                auto food = r.create();
                auto pos  = r.get<component::Position>(e).position;
                static RandomEngine rng;
                r.assign<component::Position>(food,
                                              glm::vec3(pos.x + rng.uniform(-10.f, 10.f), pos.y + rng.uniform(-10.f, 10.f), 0.f));
                r.assign<component::Sprite>(food, f_tex, glm::vec3(0.9f, 0.6f, 0.1f));
                r.assign<component::Tags>(food, TAG_Food);
            });
    }

    for (int k = 0; k < 100; k++)
    {
        auto ponds = m_registry.create();
        m_registry.assign<component::Position>(ponds,
                                               glm::vec3(m_rng.uniform(-m_scenario.bounds.x, m_scenario.bounds.x),
                                                         m_rng.uniform(-m_scenario.bounds.y, m_scenario.bounds.y),
                                                         0.f));
        m_registry.assign<component::Sprite>(ponds, t_tex, glm::vec3(0.1f, 0.2f, 0.6f));
        m_registry.assign<component::Tags>(ponds, TAG_Avoidable);
        m_registry
            .assign<component::Timer>(ponds, m_rng.uniform(15.f, 50.f), 0.f, -1, [d_tex](entt::entity e, entt::registry& r) {
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
            spdlog::warn("adding system \"{}\" that is unknown", system);
        }
    }
}

void ScenarioScene::on_exit()
{
    input::get_input().remove_context(input::KeyContext::ScenarioScene);
}

bool ScenarioScene::update(float dt)
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
                     ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);
    ImGui::PopStyleVar();

    ImGuiID dockspace_id = ImGui::GetID("Scenario##Docking");
    ImGui::DockSpace(dockspace_id,
                     {0.f, 0.f},
                     ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode |
                         ImGuiDockNodeFlags_AutoHideTabBar);

    /** Extract above to func ^ */

    ImGui::Begin("Scenario Scene");

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

    ImGui::End();

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

} // namespace cs
