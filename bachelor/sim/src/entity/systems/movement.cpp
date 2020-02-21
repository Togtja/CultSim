#include "movement.h"
#include "ai/path_finding.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/events.h"
#include "gfx/renderer.h"

#include <execution>
#include <random>

#include <gfx/ImGUI/imgui.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace cs::system
{
Movement::Movement(entt::registry& registry, entt::dispatcher& dispatcher) : ISystem(registry), m_dispatcher(dispatcher)
{
}

void Movement::update(float dt)
{
    CS_AUTOTIMER(Movement System);

    static float avoid_rotation = 35.f;
    static float avoid_cd       = 0.14f;
    static float avoid_dist     = 10.f;
    static float avoid_start    = 15.f;

    ImGui::DragFloat("Avoid Rot", &avoid_rotation, 0.5f, -180.f, 180.f);
    ImGui::DragFloat("Avoid CD", &avoid_cd, 0.01f, 0.01f, 1.f);
    ImGui::DragFloat("Avoid Dist", &avoid_dist, 1.f, 1.f, 256.f);
    ImGui::DragFloat("Avoid Start", &avoid_start, 1.f, 1.f, 64.f);

    m_registry.view<component::Vision, component::Movement, component::Position>().each(
        [&dt, this](entt::entity e, const component::Vision& vis, component::Movement& mov, component::Position& pos) {
            mov.avoidance_cd -= dt;
            if (mov.avoidance_cd > 0.f)
            {
                return;
            }

            if (mov.desired_position.empty())
            {
                return;
            }

            for (auto other : vis.seen)
            {
                if (!(m_registry.get<component::Tags>(other).tags & TAG_Avoidable))
                {
                    continue;
                }

                if (auto* other_pos = m_registry.try_get<component::Position>(other); other_pos)
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
                            ai::find_path_astar(pos.position, target, mov.desired_position, 16);
                            mov.avoid_count = 0;
                        }
                    }
                }
            }
        });

    static auto seed = std::random_device{};
    static auto gen  = std::mt19937{seed()};
    std::uniform_real_distribution rng(-1.f, 1.f);

    auto view = m_registry.view<component::Movement, component::Position>();
    view.each([dt, &rng, this](entt::entity e, component::Movement& mov, component::Position& pos) {
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

        pos.position += glm::vec3(mov.direction, 0.f) * mov.speed * dt;

        //        gfx::get_renderer().debug().draw_line(pos.position,
        //                                              pos.position + glm::vec3(mov.direction, 0.f) * mov.speed,
        //                                              {1.f, 0.f, 1.f});

        //        gfx::get_renderer().debug().draw_line(pos.position, mov.desired_position.back(), {0.f, 1.f, 1.f});
        //        for (int i = 0; i < mov.desired_position.size() - 1; ++i)
        //        {
        //            gfx::get_renderer().debug().draw_line(mov.desired_position[i], mov.desired_position[i + 1],
        //            {0.f, 1.f, 1.f});
        //        }

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
                m_dispatcher.enqueue(event::ArrivedAtDestination{e, cur_head});
            }
        }
    });
}
} // namespace cs::system
