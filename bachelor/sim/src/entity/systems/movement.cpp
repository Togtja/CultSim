#include "movement.h"
#include "ai/path_finding.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/events.h"
#include "gfx/renderer.h"

#include <execution>
#include <random>

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
    m_registry.view<component::Vision, component::Movement, component::Position>().each(
        [&dt, this](entt::entity e, const component::Vision& vis, component::Movement& mov, const component::Position& pos) {
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
                if (auto* other_mov = m_registry.try_get<component::Movement>(other); other_mov)
                {
                    if (other_mov->desired_position.empty())
                    {
                        return;
                    }

                    if (ai::world_to_grid(pos.position, 32) == ai::world_to_grid(other_mov->desired_position.back(), 32))
                    {
                        auto rot = glm::mat2(glm::cos(glm::radians(-45.f)),
                                             -glm::sin(glm::radians(-45.f)),
                                             glm::sin(glm::radians(45.f)),
                                             glm::cos(glm::radians(45.f)));

                        auto offset_dir = rot * mov.direction;
                        mov.desired_position.push_back(mov.desired_position.back() -
                                                       glm::vec3(glm::vec2(offset_dir * 16.f), 0.f));
                        mov.avoidance_cd = 0.5f;
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

        pos.position = glm::mix(pos.position, pos.position + glm::vec3(mov.direction, 0.f) * mov.speed, dt);

        gfx::get_renderer().debug().draw_line(pos.position,
                                              pos.position + glm::vec3(mov.direction, 0.f) * mov.speed,
                                              {1.f, 0.f, 1.f});

        if (glm::distance(pos.position, cur_head) < 5.f)
        {
            mov.desired_position.pop_back();
            if (mov.desired_position.empty())
            {
                // Arrived at final destination
                m_dispatcher.enqueue(event::ArrivedAtDestination{e, cur_head});
            }
        }
    });
}
} // namespace cs::system
