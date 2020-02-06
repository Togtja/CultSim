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
    return false;
}
void AI::update(float dt)
{
bool AI::close_enough(glm::vec2 pos, glm::vec2 pos2, float treshold)
{
    glm::bvec2 boolvec = glm::epsilonEqual(pos, pos2, glm::vec2(treshold));
    return boolvec.x && boolvec.y;
}
} // namespace cs::system
