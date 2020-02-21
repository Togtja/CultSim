#include "scenario_scene.h"
#include "entity/actions/action.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/tags.h"
#include "entity/systems/action.h"
#include "entity/systems/ai.h"
#include "entity/systems/mitigation.h"
#include "entity/systems/movement.h"
#include "entity/systems/need.h"
#include "entity/systems/rendering.h"
#include "entity/systems/requirement.h"
#include "gfx/renderer.h"
#include "preferences.h"

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
    ai::Need need_hunger = {static_cast<std::string>("hunger"), 3.f, 100.f, 2.f, tags::TAG_Food};
    ai::Need need_thirst = {static_cast<std::string>("thirst"), 4.f, 100.f, 3.f, tags::TAG_Drink};
    ai::Need need_sleep  = {static_cast<std::string>("sleep"), 1.f, 100.f, 1.f, tags::TAG_Sleep};

    action::Action action_eat{static_cast<std::string>("eat"),
                              tags::TAG_Find,
                              5.f,
                              0.f,
                              {},
                              [](entt::entity e, entt::entity n, entt::registry& r) {
                                  spdlog::warn("We finished action: eat on entity: {}", e);
                                  r.destroy(n);
                                  for (auto& need : r.get<component::Needs>(e).needs)
                                  {
                                      if (need.tags & tags::TAG_Food)
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
                                tags::TAG_Find,
                                2.f,
                                0.f,
                                {},
                                [](entt::entity e, entt::entity n, entt::registry& r) {
                                    spdlog::warn("We finished action: drink on entity: {}", e);
                                    r.destroy(n);
                                    for (auto& need : r.get<component::Needs>(e).needs)
                                    {
                                        if (need.tags & tags::TAG_Drink)
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
                                        if (need.tags & tags::TAG_Sleep)
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
                                      tags::TAG_Food,
                                      std::vector<action::Action>{action_eat}};
    ai::Strategy strategy_finddrink = {static_cast<std::string>("drink water"),
                                       0,
                                       {},
                                       tags::TAG_Drink,
                                       std::vector<action::Action>{action_drink}};
    ai::Strategy strategy_sleep     = {static_cast<std::string>("sleep"),
                                   0,
                                   {},
                                   tags::TAG_Sleep,
                                   std::vector<action::Action>{action_sleep}};

    static auto seed = std::random_device{};
    static auto gen  = std::mt19937{seed()};
    std::normal_distribution<float> rng(0.f, 1.f);

    auto tex   = gfx::get_renderer().sprite().get_texture("sprites/weapon_c.png");
    auto f_tex = gfx::get_renderer().sprite().get_texture("sprites/food_c.png");
    auto d_tex = gfx::get_renderer().sprite().get_texture("sprites/liquid_c.png");
    for (int i = 0; i < 100; i++)
    {
        auto agent = m_registry.create();
        int i1     = i;
        if (i % 2 == 1)
        {
            i1 = -i;
        }
        glm::vec2 pos(i1 * 15.f, 0.f);
        m_registry.assign<component::Position>(agent, glm::vec3(pos, 0));
        m_registry.assign<component::Movement>(agent,
                                               std::vector<glm::vec3>(1, glm::vec3(rng(seed) * 100, rng(seed) * 100, 0)),
                                               glm::normalize(glm::vec2(1.f, 1.f)),
                                               50.f);
        m_registry.assign<component::Sprite>(agent, tex, glm::vec3(1.f, 0.f, 0.f));
        m_registry.assign<component::Vision>(agent, std::vector<entt::entity>{}, 40.f, static_cast<uint8_t>(0));
        m_registry.assign<component::Needs>(agent,
                                            std::vector<ai::Need>{need_hunger, need_thirst, need_sleep},
                                            std::vector<ai::Need>{});

        m_registry.assign<component::Strategies>(
            agent,
            std::vector<ai::Strategy>({strategy_findfood, strategy_finddrink, strategy_sleep}),
            std::vector<ai::Strategy>{});

        m_registry.assign<component::Tags>(agent, static_cast<tags::ETag>(0));
    }
    for (int j = 0; j < 900; j++)
    {
        auto food = m_registry.create();
        m_registry.assign<component::Position>(food, glm::vec3(rng(seed) * 500, rng(seed) * 500, 0));
        m_registry.assign<component::Sprite>(food, f_tex, glm::vec3(0.5f, 0.5f, 1.f));
        m_registry.assign<component::Tags>(food, tags::TAG_Food);
    }

    for (int k = 0; k < 900; k++)
    {
        auto drink = m_registry.create();
        m_registry.assign<component::Position>(drink, glm::vec3(rng(seed) * 500, rng(seed) * 500, 0));
        m_registry.assign<component::Sprite>(drink, f_tex, glm::vec3(0.5f, 0.5f, 1.f));
        m_registry.assign<component::Tags>(drink, tags::TAG_Drink);
    }

    /** Add required systems */
    m_active_systems.emplace_back(new system::Need(m_registry));
    m_active_systems.emplace_back(new system::Mitigation(m_registry));
    m_active_systems.emplace_back(new system::Action(m_registry));
    m_active_systems.emplace_back(new system::Requirement(m_registry));
    m_active_systems.emplace_back(new system::AI(m_registry));
    m_active_systems.emplace_back(new system::Movement(m_registry, m_dispatcher));
    m_active_systems.emplace_back(new system::Rendering(m_registry));

    m_context->preferences->on_preference_changed.connect<&response>();
}

void ScenarioScene::on_exit()
{
    m_context->preferences->on_preference_changed.disconnect<&response>();
}

bool ScenarioScene::update(float dt)
{
    ImGui::Begin("Scenario Scene");
    ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

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
    for (int i = -5; i <= 5; i++)
    {
        gfx::get_renderer().debug().draw_line(glm::vec3(-32 * 5, i * 32, 0), glm::vec3(32 * 5, i * 32, 0), glm::vec3(1, 0.5f, 1));
        gfx::get_renderer().debug().draw_line(glm::vec3(i * 32, -32 * 5, 0), glm::vec3(i * 32, 32 * 5, 0), glm::vec3(1, 0.5f, 1));
    }
    return false;
}

} // namespace cs
