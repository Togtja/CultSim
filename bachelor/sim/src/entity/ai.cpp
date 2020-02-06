#include "ai.h"
#include "entity/components.h"

#include <glm/glm.hpp>

namespace cs::system
{
bool AI::is_visible(glm::vec2 pos, glm::vec2 pos2, float rad)
{
    float x = pos.x - pos2.x;
    float y = pos.y - pos2.y;
    if (x * x + y * y <= rad * rad)
    {
        return true;
}
    return false;
}
bool AI::is_colliding()
{
}
void AI::update(float dt)
{
}
} // namespace cs::system
