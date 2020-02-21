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
    ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 10.f, tags::TAG_Food};

    action::Action action{static_cast<std::string>("eat"),
                          tags::TAG_Find,
                          5.f,
                          0.f,
                          []() { spdlog::warn("We finished action: eat"); },
                          []() { spdlog::warn("We failed to finish action: eat"); },
                          {}};

    ai::Strategy strategy = {static_cast<std::string>("eat food"),
                             0,
                             {},
                             tags::TAG_Food,
                             std::vector<action::Action>{std::move(action)}};

    RandomEngine rng{};

    auto tex   = gfx::get_renderer().sprite().get_texture("sprites/weapon_c.png");
    auto f_tex = gfx::get_renderer().sprite().get_texture("sprites/food_c.png");

    for (int i = 0; i < 50000; i++)
    {
        auto agent = m_registry.create();
        int i1     = i;
        if (i % 2 == 1)
        {
            i1 = -i;
        }
        glm::vec2 pos(rng.uniform(-500.f, 500.f), rng.uniform(-500.f, 500.f));
        m_registry.assign<component::Position>(agent, glm::vec3(pos, 0));
        m_registry.assign<component::Movement>(agent, std::vector<glm::vec3>{}, glm::vec2{}, 100.f, 0.f);
        m_registry.assign<component::Sprite>(agent, tex, glm::vec3(1.f, 0.f, 0.f));
        m_registry.assign<component::Vision>(agent, std::vector<entt::entity>{}, 40.f, static_cast<uint8_t>(0));
        m_registry.assign<component::Needs>(agent, std::vector<ai::Need>{need}, std::vector<ai::Need>{});
        m_registry.assign<component::Strategies>(agent, std::vector<ai::Strategy>({strategy}), std::vector<ai::Strategy>{});
        m_registry.assign<component::Tags>(agent, static_cast<tags::ETag>(0));
    }

    auto food = m_registry.create();
    m_registry.assign<component::Position>(food, glm::vec3(100, 100, 0));
    m_registry.assign<component::Sprite>(food, f_tex, glm::vec3(0.5f, 0.5f, 1.f));
    m_registry.assign<component::Tags>(food, tags::TAG_Food);

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
    static bool show_grid = true;
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
