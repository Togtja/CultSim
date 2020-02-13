#include "move_to.h"

namespace cs::action
{
void MoveTo::success()
{
    // We reached our goal, remove us from the stack
}
void MoveTo::failure()
{
    // We failed to reach our goal, recalculate goals
}

void MoveTo::update(float dt)
{
    // Set movement.desired_pos to be equal to m_pos
}
} // namespace cs::action
