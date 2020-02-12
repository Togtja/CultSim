#include "ai.h"
#include "constants.h"
#include "entity/components.h"

#include <unordered_map>

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
    std::unordered_map<uint32_t, std::vector<entt::entity>> collision_grid{};

    /** Construct collision grid */

    m_registry.view<component::Position>().each([&collision_grid, this](entt::entity e, const component::Position& pos) {
        auto min = world_to_grid(pos.position - glm::vec3(50.f, 50.f, 0));
        auto max = world_to_grid(pos.position + glm::vec3(50.f, 50.f, 0));
        for (int x = min.x; x <= max.x; x++)
        {
            for (int y = min.y; y <= max.y; y++)
            {
                collision_grid[x * SIM_GRID_SIZE + y].emplace_back(e);
            }
        }
    });

    m_registry.view<component::Vision>().each([](component::Vision& vis) { vis.seen.clear(); });
    auto vis_view = m_registry.group<component::Vision>(entt::get<component::Position>);

    vis_view.each([this, &collision_grid](entt::entity e, component::Vision& vis, const component::Position& pos) {
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
                        vis.seen.push_back(e);
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
