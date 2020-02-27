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
#include "entity/systems/requirement.h"
#include "entity/systems/timer.h"
#include "gfx/renderer.h"
#include "preferences.h"
#include "random_engine.h"

#include <common_helpers.h>
#include <functional>
#include <memory>
#include <random>

#include "gfx/ImGUI/imgui.h"
#include "spdlog/spdlog.h"

namespace cs
{
static void response(const Preference& before, const Preference& after)
{
    spdlog::info("preference changed [{}]", before.name);
}

ScenarioScene::ScenarioScene(std::string_view scenario)
{
}

void ScenarioScene::on_enter()
{
    ai::Need need_hunger = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, TAG_Food};
    ai::Need need_thirst = {static_cast<std::string>("thirst"), 4.f, 100.f, 1.5f, TAG_Drink};
    ai::Need need_sleep  = {static_cast<std::string>("sleep"), 1.f, 100.f, 0.5f, TAG_Sleep};

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
                                          need.status += 50.f;
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
                                            need.status += 50.f;
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

    RandomEngine rng{};

    auto tex   = gfx::get_renderer().sprite().get_texture("sprites/agent_c.png");
    auto f_tex = gfx::get_renderer().sprite().get_texture("sprites/food_c.png");
    auto d_tex = gfx::get_renderer().sprite().get_texture("sprites/liquid_c.png");
    auto t_tex = gfx::get_renderer().sprite().get_texture("sprites/circle.png");

    for (int i = 1; i <= 1; i++)
    {
        auto agent = m_registry.create();
        int i1     = i;
        if (i % 2 == 1)
        {
            i1 = -i;
        }
        glm::vec2 pos(rng.uniform(-500.f, 500.f), rng.uniform(-500.f, 500.f));
        m_registry.assign<component::Position>(agent, glm::vec3(pos, 0));
        m_registry.assign<component::Movement>(agent, std::vector<glm::vec3>{}, glm::vec2{}, 80.f, 0.f);
        m_registry.assign<component::Sprite>(agent, tex, glm::vec3(1.f, 0.f, 0.f));
        m_registry.assign<component::Vision>(agent, std::vector<entt::entity>{}, 40.f, static_cast<uint8_t>(0));
        m_registry.assign<component::Tags>(agent, TAG_Avoidable);
        m_registry.assign<component::Needs>(agent,
                                            std::vector<ai::Need>{need_hunger, need_thirst, need_sleep},
                                            std::vector<ai::Need>{});

        m_registry.assign<component::Strategies>(
            agent,
            std::vector<ai::Strategy>({strategy_findfood, strategy_finddrink, strategy_sleep}),
            std::vector<ai::Strategy>{});
        m_registry.assign<component::Health>(agent, 100.f, 1.f, ETag(TAG_Food | TAG_Drink | TAG_Sleep));
    }

    for (int j = 0; j < 50; j++)
    {
        auto trees = m_registry.create();
        m_registry.assign<component::Position>(trees, glm::vec3(rng.uniform(-500.f, 500.f), rng.uniform(-500.f, 500.f), 0.f));
        m_registry.assign<component::Sprite>(trees, t_tex, glm::vec3(0.8f, 0.5f, 0.1f));
        m_registry.assign<component::Tags>(trees, TAG_Avoidable);
        m_registry.assign<component::Timer>(trees, rng.uniform(25.f, 100.f), 0.f, -1, [f_tex](entt::entity e, entt::registry& r) {
            auto food = r.create();
            auto pos  = r.get<component::Position>(e).position;
            static RandomEngine rng;
            r.assign<component::Position>(food,
                                          glm::vec3(pos.x + rng.uniform(-10.f, 10.f), pos.y + rng.uniform(-10.f, 10.f), 0.f));
            r.assign<component::Sprite>(food, f_tex, glm::vec3(0.9f, 0.6f, 0.1f));
            r.assign<component::Tags>(food, TAG_Food);
        });
    }

    for (int k = 0; k < 25; k++)
    {
        auto ponds = m_registry.create();
        m_registry.assign<component::Position>(ponds, glm::vec3(rng.uniform(-500.f, 500.f), rng.uniform(-500.f, 500.f), 0.f));
        m_registry.assign<component::Sprite>(ponds, t_tex, glm::vec3(0.1f, 0.2f, 0.6f));
        m_registry.assign<component::Tags>(ponds, TAG_Avoidable);
        m_registry.assign<component::Timer>(ponds, rng.uniform(15.f, 50.f), 0.f, -1, [d_tex](entt::entity e, entt::registry& r) {
            auto drink = r.create();
            auto pos   = r.get<component::Position>(e).position;
            static RandomEngine rng;
            r.assign<component::Position>(drink,
                                          glm::vec3(pos.x + rng.uniform(-10.f, 10.f), pos.y + rng.uniform(-10.f, 10.f), 0.f));
            r.assign<component::Sprite>(drink, d_tex, glm::vec3(0.1f, 0.7f, 1.f));
            r.assign<component::Tags>(drink, TAG_Drink);
        });
    }

    /** Add required systems */
    m_active_systems.emplace_back(new system::Need(m_registry));
    m_active_systems.emplace_back(new system::Health(m_registry));
    m_active_systems.emplace_back(new system::Mitigation(m_registry));
    m_active_systems.emplace_back(new system::Action(m_registry));
    m_active_systems.emplace_back(new system::Requirement(m_registry));
    m_active_systems.emplace_back(new system::AI(m_registry));
    m_active_systems.emplace_back(new system::Movement(m_registry, m_dispatcher));
    m_active_systems.emplace_back(new system::Rendering(m_registry));
    m_active_systems.emplace_back(new system::Timer(m_registry));

    m_context->preferences->on_preference_changed.connect<&response>();
}

void ScenarioScene::on_exit()
{
    m_context->preferences->on_preference_changed.disconnect<&response>();
}

bool ScenarioScene::update(float dt)
{
    ImGui::Begin("Scenario Scene");

    static auto b_tex = gfx::get_renderer().sprite().get_texture("sprites/background_c.png");
    b_tex.scale       = 100.f;

    for (int i = -50; i < 50; i++)
    {
        for (int j = -50; j < 50; j++)
        {
            gfx::get_renderer().sprite().draw(glm::vec3(i * 100.f, j * 100.f, -1.f), glm::vec3(0.f, 0.3f, 0.1f), b_tex);
        }
    }

    for (auto& system : m_active_systems)
    {
        system->update(dt);
    }

    ImGui::End();

    /** Deal with long running tasks, then events */
    m_scheduler.update(dt);
    m_dispatcher.update();

    return false;
}

bool ScenarioScene::draw()
{
    ImGui::Text("GRID");
    ImGui::Separator();
    static bool show_grid = false;
    static int grid_span  = 25;
    static int grid_size  = 32;
    ImGui::Checkbox("Show Grid", &show_grid);
    ImGui::DragInt("Grid Span", &grid_span, 1.f, 0, 50);
    ImGui::DragInt("Grid Size", &grid_size, 1.f, 0, 256);

    if (show_grid)
    {
        for (int i = -grid_span; i <= grid_span; i++)
        {
            gfx::get_renderer().debug().draw_line(glm::vec3(-grid_size * grid_span, i * grid_size, 0),
                                                  glm::vec3(grid_size * grid_span, i * grid_size, 0),
                                                  glm::vec3(0.3f));

            gfx::get_renderer().debug().draw_line(glm::vec3(i * grid_size, -grid_size * grid_span, 0),
                                                  glm::vec3(i * grid_size, grid_size * grid_span, 0),
                                                  glm::vec3(0.3f));
        }
    }
    return false;
}

} // namespace cs
