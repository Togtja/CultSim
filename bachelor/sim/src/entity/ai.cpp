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

void AI::update(float dt)
{
    std::unordered_map<uint32_t, std::vector<entt::entity>> collision_grid{};

    auto hash = [](glm::vec2 pos) -> glm::ivec2 {
        return {static_cast<int>(pos.x) / static_cast<int>(SIM_GRID_SIZE),
                static_cast<int>(pos.y) / static_cast<int>(SIM_GRID_SIZE)};
    };

    /** Construct collision grid */

    m_registry.view<component::Position>().each([&collision_grid, &hash](entt::entity e, const component::Position& pos) {
        auto min = hash(pos.position);
        collision_grid[min.x * SIM_GRID_SIZE + min.y].emplace_back(e);
    });

    m_registry.view<component::Vision>().each([](component::Vision& vis) { vis.seen.clear(); });
    auto vis_view = m_registry.group<component::Vision>(entt::get<component::Position>);
    for (auto& [hash, cell] : collision_grid)
    {
        /* Don't bother if there are less than two motes in the cell */
        if (cell.size() < 2)
        {
            continue;
        }

        /* Loop through all motes in that cell */
        for (size_t i = 0u; i < cell.size() - 1; ++i)
        {
            auto&& [apos, avis] = vis_view.get<component::Vision, component::Position>(cell[i]);
            for (size_t j = i + 1u; j < cell.size(); ++j)
            {
                /* Detect if there is a collision */
                auto&& [bpos, bvis] = vis_view.get<component::Vision, component::Position>(cell[i]);
                if (close_enough(avis.position, bvis.position, apos.vision_radius))
                {
                    apos.seen.push_back(cell[j]);
                }
                if (close_enough(bvis.position, avis.position, bpos.vision_radius))
                {
                    bpos.seen.push_back(cell[i]);
                }
            }
        }
    }
}

bool AI::close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold)
{
    glm::bvec2 boolvec = glm::epsilonEqual(pos, pos2, glm::vec2(threshold));
    return boolvec.x && boolvec.y;
}

} // namespace cs::system
