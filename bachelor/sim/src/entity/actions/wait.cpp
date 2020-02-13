#include "wait.h"

namespace cs::action
{
void Wait::update(float dt)
{
    m_time_left -= dt;
    if (m_time_left <= 0.f)
    {
        success();
    }
}
} // namespace cs::action
