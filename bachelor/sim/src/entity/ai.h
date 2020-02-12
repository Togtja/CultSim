#pragma once
#include "entity/system.h"

#include <random>

#include <glm/vec2.hpp>

namespace cs::system
{
class AI : public ISystem
{
public:
    using ISystem::ISystem;

    bool is_visible(glm::vec2 pos, glm::vec2 pos2, float rad);

    bool is_colliding(glm::vec2 pos, glm::vec2 pos2, float size, float size2);

    glm::vec2 path_finding();

    glm::ivec2 world_to_grid(glm::vec2 pos);

    void update(float dt) override;

private:
    bool close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold);
};

} // namespace cs::system
