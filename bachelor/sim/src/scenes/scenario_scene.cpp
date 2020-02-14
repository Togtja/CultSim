#include "scenario_scene.h"
#include "entity/actions/action.h"
#include "entity/actions/requirement.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/tags.h"
#include "entity/systems/action.h"
#include "entity/systems/ai.h"
#include "entity/systems/mitigation.h"
#include "entity/systems/movement.h"
#include "entity/systems/need.h"
#include "entity/systems/rendering.h"
#include "gfx/renderer.h"

#include <functional>
#include <random>

#include "gfx/ImGUI/imgui.h"
#include "spdlog/spdlog.h"

namespace cs
{
ScenarioScene::ScenarioScene(std::string_view scenario)
{
}

void ScenarioScene::on_enter()
{
    ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, tags::TAG_Food};
    action::Action action{static_cast<std::string>("eat"),
                          std::vector<action::Requirement>{},
                          5.f,
                          0.f,
                          {},
                          []() { spdlog::warn("We finished action: eat"); },
                          []() {
                              spdlog::warn("We failed to finish action: eat");
                          }};
    ai::Strategy strategy = {static_cast<std::string>("eat food"), 0, {}, tags::TAG_Food, std::vector<action::Action>({action})};

    static auto seed = std::random_device{};
    static auto gen  = std::mt19937{seed()};
    std::normal_distribution<float> rng(0.f, 1.f);

    auto tex = gfx::get_renderer().sprite().get_texture("sprites/weapon_c.png");
    for (int i = 0; i < 100; i++)
    {
        auto agent = m_registry.create();
        glm::vec2 pos(i * 15.f, 0.f);

        m_registry.assign<component::Position>(agent, glm::vec3(pos, 0), glm::vec3(0, 0, 0));
        m_registry.assign<component::Movement>(agent, glm::vec2(0.f, 0.f), glm::normalize(glm::vec2(1.f, 1.f)), 50.f);
        m_registry.assign<component::Sprite>(agent, tex, glm::vec3(1.f, 0.f, 0.f));
        m_registry.assign<component::Vision>(agent, std::vector<entt::entity>{}, 40.f, static_cast<uint8_t>(0));
        m_registry.assign<component::Needs>(agent, std::vector<ai::Need>({need}), std::vector<ai::Need>({}));
        m_registry.assign<component::Strategies>(agent, std::vector<ai::Strategy>({strategy}), std::vector<ai::Strategy>({}));
        m_registry.assign<component::Requirement>(agent, std::vector<action::Requirement>({}));
        m_registry.assign<component::Tags>(agent, tags::TAG_Food);
    }

    /** Add required systems */
    m_active_systems.emplace_back(new system::Need(m_registry));
    m_active_systems.emplace_back(new system::Mitigation(m_registry));
    m_active_systems.emplace_back(new system::Action(m_registry, m_dispatcher));
    m_active_systems.emplace_back(new system::AI(m_registry));
    m_active_systems.emplace_back(new system::Movement(m_registry, m_dispatcher));
    m_active_systems.emplace_back(new system::Rendering(m_registry));
}

void ScenarioScene::on_exit()
{
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
    return false;
}

} // namespace cs
