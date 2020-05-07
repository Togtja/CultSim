#include "movement.h"
#include "ai/path_finding.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/events.h"
#include "gfx/renderer.h"

#include <random>

#include <gfx/ImGUI/imgui.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace cs::system
{
/** TODO: Fix out of bounds functions */
bool fix_out_of_bounds(glm::vec3& pos, const glm::vec2& bounds)
{
    /** Handle wrapping */
    bool wrap = false;
    if (pos.x < -bounds.x)
    {
        pos.x = bounds.x;
        wrap  = true;
    }
    else if (pos.x > bounds.x)
    {
        pos.x = -bounds.x;
        wrap  = true;
    }
    if (pos.y < -bounds.y)
    {
        pos.y = bounds.y;
        wrap  = true;
    }
    else if (pos.y > bounds.y)
    {
        pos.y = -bounds.y;
        wrap  = true;
    }
    return wrap;
}

bool out_of_bounds(const glm::vec3& pos, const glm::vec2& bounds)
{
    auto cpy = pos;
    return fix_out_of_bounds(cpy, bounds);
}

/** TODO: Add comments */
void Movement::update(float dt)
{
    CS_AUTOTIMER(Movement System);

    auto& registry = *m_context.registry;

    registry.view<component::Vision, component::Movement, component::Position>().each(
        [&dt, this, &registry](entt::entity e, const component::Vision& vis, component::Movement& mov, component::Position& pos) {
            mov.avoidance_cd -= dt;
            if (mov.avoidance_cd > 0.f)
            {
                return;
            }

            if (mov.desired_position.empty())
            {
                return;
            }

            for (const auto other : vis.seen)
            {
                if (!(registry.get<component::Tags>(other).tags & TAG_Avoidable))
                {
                    continue;
                }

                if (auto* other_pos = registry.try_get<component::Position>(other); other_pos)
                {
                    if (glm::distance(pos.position, other_pos->position) < 12.f)
                    {
                        auto diff = glm::normalize(other_pos->position - pos.position);
                        pos.position -= diff * dt * 36.f;
                        continue;
                    }

                    if (glm::distance(pos.position, other_pos->position) < avoid_start)
                    {
                        const float avoid_rot = glm::radians(avoid_rotation);
                        auto rot = glm::mat2(glm::cos(avoid_rot), -glm::sin(avoid_rot), glm::sin(avoid_rot), glm::cos(avoid_rot));

                        auto offset_dir = rot * mov.direction;
                        mov.desired_position.push_back(pos.position + glm::vec3(glm::vec2(offset_dir * avoid_dist), 0.f));
                        mov.avoidance_cd = avoid_cd;
                        mov.avoid_count += 1;

                        if (mov.avoid_count > 6)
                        {
                            auto target = mov.desired_position.front();
                            mov.desired_position.clear();
                            ai::find_path_astar(pos.position, target, mov.desired_position, m_context.scenario->bounds, 16);
                            mov.avoid_count = 0;
                        }
                    }
                }
            }
        });

    auto view = registry.view<component::Movement, component::Position>();
    view.each([dt, this](entt::entity e, component::Movement& mov, component::Position& pos) {
        if (mov.desired_position.empty())
        {
            return;
        }
        auto cur_head  = mov.desired_position.back();
        glm::vec3 temp = cur_head - pos.position;

        if (glm::length(temp) != 0)
        {
            mov.direction = glm::normalize(temp);
        }
        else
        {
            mov.direction = temp;
        }

        pos.position += glm::vec3(mov.direction, 0.f) * mov.speed * mov.speed_multi * dt;

        /** If we wrapped around we recalculate the path */
        if (fix_out_of_bounds(pos.position, m_context.scenario->bounds))
        {
            cur_head = mov.desired_position.front();
            mov.desired_position.clear();
            ai::find_path_astar(pos.position, cur_head, mov.desired_position, m_context.scenario->bounds);
        }

        if (glm::distance(pos.position, cur_head) < 5.f)
        {
            mov.desired_position.pop_back();
            if (mov.avoid_count > 0)
            {
                --mov.avoid_count;
            }

            if (mov.desired_position.empty())
            {
                // Arrived at final destination
                m_context.dispatcher->enqueue(event::ArrivedAtDestination{e, cur_head});
            }
        }
    });
}

void Movement::update_imgui()
{
    static bool draw_paths     = false;
    static bool draw_direction = false;

    if (ImGui::TreeNode("Path Adjustments"))
    {
        ImGui::Checkbox("Draw Paths", &draw_paths);
        ImGui::Checkbox("Draw Direction", &draw_direction);
        ImGui::DragFloat("Avoid Rot", &avoid_rotation, 0.5f, -180.f, 180.f);
        ImGui::DragFloat("Avoid CD", &avoid_cd, 0.01f, 0.01f, 1.f);
        ImGui::DragFloat("Avoid Dist", &avoid_dist, 1.f, 1.f, 256.f);
        ImGui::DragFloat("Avoid Start", &avoid_start, 1.f, 1.f, 64.f);
        ImGui::TreePop();
    }

    auto view = m_context.registry->view<component::Movement, component::Position>();
    view.each([this](entt::entity e, const component::Movement& mov, const component::Position& pos) {
        if (draw_direction)
        {
            gfx::get_renderer().debug().draw_line(pos.position,
                                                  pos.position + glm::vec3(mov.direction, 0.f) * mov.speed,
                                                  {1.f, 0.f, 1.f});
        }

        /** Draw paths */
        if (draw_paths)
        {
            if (!mov.desired_position.empty())
            {
                gfx::get_renderer().debug().draw_line(pos.position, mov.desired_position.back(), {0.f, 1.f, 1.f});
                gfx::get_renderer().debug().draw_circle(mov.desired_position.front(), 3, {0.f, 1.f, 0.f});
            }

            for (int i = mov.desired_position.size() - 1; i > 0; --i)
            {
                auto dst        = mov.desired_position[i - 1];
                auto src        = mov.desired_position[i];
                auto bounds     = m_context.scenario->bounds; // ai::world_to_grid(m_context.scenario->bounds, 32) * 32;
                glm::vec3 color = {0.f, 1.f, 1.f};
                /** TODO: Fix issue with multi wrapping, and/or refactor the entire thing */
                if (out_of_bounds(dst, bounds) && !out_of_bounds(src, bounds))
                {
                    color = {0.f, 1.f, 0.f};
                    gfx::get_renderer().debug().draw_line(src, dst, color);

                    gfx::get_renderer().debug().draw_circle(src, 3, {1.f, 0.f, 0.f});
                    /** Draw indication we are leaving map */
                    /** gfx::get_renderer().debug().draw_line(src, dst, color); */
                    for (int j = i - 1; j > 2; --j)
                    {
                        auto newsrc = ai::world_to_grid_bound(mov.desired_position[j], SIM_GRID_SIZE, bounds) * SIM_GRID_SIZE;
                        auto newdst = ai::world_to_grid_bound(mov.desired_position[j - 1], SIM_GRID_SIZE, bounds) * SIM_GRID_SIZE;

                        gfx::get_renderer().debug().draw_circle(newsrc, 3, {1.f, 1.f, 0.f});

                        gfx::get_renderer().debug().draw_line(glm::vec3(newdst, 0), glm::vec3(newsrc, 0), {0.f, 0.f, 1.f});
                    }
                    auto newdst = ai::world_to_grid_bound(mov.desired_position[2], SIM_GRID_SIZE, bounds) * SIM_GRID_SIZE;

                    auto newsrc = glm::vec3(newdst, 0) - (dst - src);
                    gfx::get_renderer().debug().draw_circle(newsrc, 3, {1.f, 0.f, 0.f});
                    gfx::get_renderer().debug().draw_line(newsrc, glm::vec3(newdst, 0), color);

                    gfx::get_renderer().debug().draw_circle(newdst, 3, {1.f, 1.f, 0.f});
                    gfx::get_renderer().debug().draw_line(glm::vec3(newdst, 0), mov.desired_position.front(), color);
                    break;
                }
                else if (out_of_bounds(src, bounds))
                {
                    continue;
                }
                gfx::get_renderer().debug().draw_circle(src, 3, {1.f, 1.f, 0.f});
                gfx::get_renderer().debug().draw_line(dst, src, color);
            }
        }
    });
}

ISystem* Movement::clone()
{
    return new Movement(m_context);
}
} // namespace cs::system
