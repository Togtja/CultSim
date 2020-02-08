#include "scenario_scene.h"
#include "entity/components.h"
#include "gfx/renderer.h"
#include "entity/ai.h"
#include "entity/movement.h"
#include "entity/rendering.h"

namespace cs
{
ScenarioScene::ScenarioScene(std::string_view scenario)
{
}

void ScenarioScene::on_enter()
{
    auto tex = gfx::get_renderer().sprite().get_texture("sprites/A_human_token.png");
    for (int i = 0; i < 100; i++)
    {
        auto agent = m_registry.create();
        glm::vec2 pos(i * 15, 0);

        m_registry.assign<component::Position>(agent, glm::vec3(pos, 0), glm::vec3(0, 0, 0));
        m_registry.assign<component::Movement>(agent, glm::vec2(0.f, 0.f), glm::normalize(glm::vec2(1.f, 1.f)), 25.f);
        m_registry.assign<component::Sprite>(agent, tex, glm::vec3(1.f, 0.f, 0.f));
        m_registry.assign<component::Vision>(agent, 40.f, static_cast<uint8_t>(0));
    }

    /** Add required systems */
    m_active_systems.emplace_back(system::AI(m_registry));
    m_active_systems.emplace_back(system::Movement(m_registry));
    m_active_systems.emplace_back(system::Rendering(m_registry));
}

void ScenarioScene::on_exit()
{
}

bool ScenarioScene::update(float dt)
{
    for(auto& system : m_active_systems)
    {
        system->update(dt);
    }

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
