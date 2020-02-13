#include "ai.h"
#include "constants.h"
#include "entity/components/components.h"

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

namespace cs::system
{
bool AI::is_visible(glm::vec2 pos, glm::vec2 pos2, float rad)
{
    float x = pos.x - pos2.x;
    float y = pos.y - pos2.y;
    return x * x + y * y <= rad * rad;
}

bool AI::is_colliding(glm::vec2 pos, glm::vec2 pos2, float size, float size2)
{
    return is_visible(pos, pos2, size + size2);
}

glm::vec2 AI::path_finding()
{
    static std::random_device rd{};
    static std::mt19937 gen(rd());
    std::uniform_int_distribution dist(-360, 360);
    return glm::vec2(dist(gen), dist(gen));
}

glm::ivec2 AI::world_to_grid(glm::vec2 pos)
{
    return {static_cast<int>(pos.x) / static_cast<int>(SIM_GRID_SIZE), static_cast<int>(pos.y) / static_cast<int>(SIM_GRID_SIZE)};
}

void AI::update(float dt)
{
    collision_grid.clear();

    /** Construct collision grid */
    m_registry.view<component::Position>().each([this](entt::entity e, const component::Position& pos) {
        auto min = world_to_grid(pos.position);
        collision_grid[min.x * SIM_GRID_SIZE + min.y].emplace_back(e);
    });

    m_registry.view<component::Vision>().each([](component::Vision& vis) { vis.seen.clear(); });
    auto vis_view = m_registry.group<component::Vision, component::Position>();

    vis_view.each([this](entt::entity e, component::Vision& vis, const component::Position& pos) {
        auto min = world_to_grid(pos.position - glm::vec3(vis.vision_radius, vis.vision_radius, 0));
        auto max = world_to_grid(pos.position + glm::vec3(vis.vision_radius, vis.vision_radius, 0));
        for (int x = min.x; x <= max.x; x++)
        {
            for (int y = min.y; y <= max.y; y++)
            {
                for (auto&& e2 : collision_grid[x * SIM_GRID_SIZE + y])
                {
                    auto& pos2 = m_registry.get<component::Position>(e2);
                    if (e == e2)
                    {
                        continue;
                    }
                    if (is_visible(pos.position, pos2.position, vis.vision_radius))
                    {
                        vis.seen.push_back(e2);

                        // Collision avoidance
                        if (is_visible(pos.position, pos2.position, vis.vision_radius * 0.5f))
                        {
                            auto move = m_registry.try_get<component::Movement>(e);
                            if (move == nullptr)
                            {
                                continue;
                            }
                            // How much ahead we see
                            auto ahead = glm::vec2(pos.position.x, pos.position.y) + move->direction * (vis.vision_radius / 2);
                            // 10 should be the size of the entt on pos2
                            if (is_visible(ahead, pos2.position, 5))
                            {
                                auto avoid_force = ahead - glm::vec2(pos2.position.x, pos2.position.y);
                                move->avoidance  = glm::normalize(avoid_force);
                                move->avoidance *= 1;
                            }
                        }
                    }
                }
            }
        }
    });
}

bool AI::close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold)
{
    glm::bvec2 boolvec = glm::epsilonEqual(pos, pos2, glm::vec2(threshold));
    return boolvec.x && boolvec.y;
}

} // namespace cs::system
