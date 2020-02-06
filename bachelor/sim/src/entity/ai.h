#pragma once
#include "entity/system.h"
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
    void update(float dt) override;
private:
    bool close_enough(glm::vec2 pos, glm::vec2 pos2, float treshold);
};

} // namespace cs::system
