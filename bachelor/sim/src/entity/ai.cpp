#include "ai.h"
#include "entity/components.h"

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

bool AI::is_colliding()
{
    return false;
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
    auto view = m_registry.view<component::Position, component::Movement, component::Sprite, component::Vision>();
    view.each([dt, this](entt::entity et,
                         component::Position& pos,
                         component::Movement& mov,
                         component::Sprite& spr,
                         component::Vision& vis) {
        auto view2 = m_registry.view<component::Position>();
        if (close_enough(pos.position, pos.desired_position, 1.f))
        {
            pos.desired_position = glm::vec3(path_finding(), 0);
        }
        for (auto et2 : view2)
        {
            if (et == et2)
            {
                continue;
            }
            // is_visible(pos.position, view2.get(et2).position, vis.vision_radius)
            // glm::distance(pos.position, view2.get(et2).position) < vis.vision_radius
            if (is_visible(pos.position, view2.get(et2).position, vis.vision_radius))
            {
                // Found eachother
                spr.color.r = 0;
                spr.color.g = 1;
                break;
            }
            else
            {
                spr.color.r = 1;
                spr.color.g = 0;
            }
        }
    });
}

bool AI::close_enough(glm::vec2 pos, glm::vec2 pos2, float treshold)
{
    glm::bvec2 boolvec = glm::epsilonEqual(pos, pos2, glm::vec2(treshold));
    return boolvec.x && boolvec.y;
}

} // namespace cs::system
