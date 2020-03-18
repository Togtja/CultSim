#include "sensor.h"
#include "ai/path_finding.h"
#include "constants.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "gfx/renderer.h"

#include <algorithm>
#include <execution>

#include <gfx/ImGUI/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

namespace cs::system
{
bool Sensor::is_visible(const glm::vec2& pos, const glm::vec2& pos2, float rad)
{
    float x = pos.x - pos2.x;
    float y = pos.y - pos2.y;
    if (x * x + y * y <= rad * rad)
    {
        return true;
    }
}

bool Sensor::is_visible_bound(const glm::vec2& pos, const glm::vec2& pos2, float rad, const glm::vec2& bounds)
{
    if (is_visible(pos, pos2, rad))
    {
        return true;
    }
    // Check for out of bound vision wrapping
    float x;
    float y;
    float rad2 = rad * rad;
    if (pos.x - rad < -bounds.x)
    {
        auto b_pos = pos + glm::vec2(2.f * bounds.x, 0);
        x          = b_pos.x - pos2.x;
        y          = b_pos.y - pos2.y;
        if (x * x + y * y <= rad2)
        {
            return true;
        }
    }
    if (pos.x + rad > bounds.x)
    {
        auto b_pos = pos - glm::vec2(2.f * bounds.x, 0);
        x          = b_pos.x - pos2.x;
        y          = b_pos.y - pos2.y;
        if (x * x + y * y <= rad2)
        {
            return true;
        }
    }
    if (pos.y - rad < -bounds.y)
    {
        auto b_pos = pos + glm::vec2(0, 2.f * bounds.y);
        x          = b_pos.x - pos2.x;
        y          = b_pos.y - pos2.y;
        if (x * x + y * y <= rad2)
        {
            return true;
        }
    }
    if (pos.y + rad > bounds.y)
    {
        auto b_pos = pos - glm::vec2(0, 2.f * bounds.y);
        x          = b_pos.x - pos2.x;
        y          = b_pos.y - pos2.y;
        if (x * x + y * y <= rad2)
        {
            return true;
        }
    }
    return false;
}

bool Sensor::is_colliding(const glm::vec2& pos, const glm::vec2& pos2, float size, float size2)
{
    return is_visible(pos, pos2, size + size2);
}

bool Sensor::is_colliding_bound(const glm::vec2& pos, const glm::vec2& pos2, float size, float size2, const glm::vec2& bounds)
{
    return is_visible_bound(pos, pos2, size + size2, bounds);
}

void Sensor::update(float dt)
{
    CS_AUTOTIMER(Sensor System);
    static bool draw_fov  = false;
    static bool draw_seen = false;

    if (ImGui::TreeNode("Sensor Info"))
    {
        ImGui::Checkbox("Draw FoV", &draw_fov);
        ImGui::Checkbox("Draw Vision Lines", &draw_seen);
        ImGui::TreePop();
    }

    m_collision_grid.clear();
    auto& registry = *m_context.registry;

    /** Construct collision grid */
    registry.view<component::Position>().each([this](entt::entity e, const component::Position& pos) {
        auto min = ai::world_to_grid_bound(pos.position, SIM_GRID_SIZE, m_context.scenario->bounds);
        m_collision_grid[min.x * SIM_GRID_SIZE + min.y].emplace_back(e);
    });

    registry.view<component::Vision>().each([](component::Vision& vis) { vis.seen.clear(); });
    auto vis_view = registry.group<component::Vision, component::Position>();

    std::for_each(std::execution::par, vis_view.begin(), vis_view.end(), [this, &registry, &vis_view](entt::entity e) {
        auto&& vis      = vis_view.get<component::Vision>(e);
        const auto& pos = vis_view.get<component::Position>(e);

        auto min = ai::world_to_grid(pos.position - glm::vec3(vis.radius, vis.radius, 0), SIM_GRID_SIZE);
        auto max = ai::world_to_grid(pos.position + glm::vec3(vis.radius, vis.radius, 0), SIM_GRID_SIZE);

        for (int x = min.x; x <= max.x; x++)
        {
            for (int y = min.y; y <= max.y; y++)
            {
                auto bounded = ai::world_to_grid_bound(glm::vec2(x * SIM_GRID_SIZE, y * SIM_GRID_SIZE),
                                                       SIM_GRID_SIZE,
                                                       m_context.scenario->bounds);

                if (m_collision_grid.find(bounded.x * SIM_GRID_SIZE + bounded.y) == m_collision_grid.end())
                {
                    continue;
                }

                for (auto e2 : m_collision_grid[bounded.x * SIM_GRID_SIZE + bounded.y])
                {
                    auto& pos2 = registry.get<component::Position>(e2);
                    if (e == e2)
                    {
                        continue;
                    }
                    if (is_visible_bound(pos.position, pos2.position, vis.radius, m_context.scenario->bounds))
                    {
                        vis.seen.push_back(e2);
                    }
                }
            }
        }
    });

    /** Debug drawing */
    if (draw_fov)
    {
        vis_view.each(
            [this, &renderer = gfx::get_renderer().debug()](const component::Vision& vis, const component::Position& pos) {
                renderer.draw_circle(pos.position, vis.radius, {.33f, .33f, .33f});

                if (draw_seen)
                {
                    for (auto seen : vis.seen)
                    {
                        renderer.draw_line(pos.position,
                                           m_context.registry->get<component::Position>(seen).position,
                                           {0.2f, 1.f, 0.2f});
                    }
                }
            });
    }
}

bool Sensor::close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold)
{
    glm::bvec2 boolvec = glm::epsilonEqual(pos, pos2, glm::vec2(threshold));
    return boolvec.x && boolvec.y;
}

} // namespace cs::system
