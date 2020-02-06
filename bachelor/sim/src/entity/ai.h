#pragma once
#include "entity/system.h"

#include <random>

#include <glm/vec2.hpp>

namespace cs::system
{
class AI : public ISystem
{
private:
public:
    using ISystem::ISystem;
    bool is_visible(glm::vec2 pos, glm::vec2 pos2, float rad);
    bool is_colliding();
    glm::vec2 path_finding()
    {
        static std::random_device rd{};
        static std::mt19937 gen(rd());
        std::uniform_int_distribution dist(-360, 360);
        return glm::vec2(dist(gen), dist(gen));
    }
    void update(float dt) override;

private:
    bool close_enough(glm::vec2 pos, glm::vec2 pos2, float treshold);
};

} // namespace cs::system
