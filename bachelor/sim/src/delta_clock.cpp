#include "delta_clock.h"

namespace cs
{
DeltaClock::DeltaClock() : m_last_reset(std::chrono::steady_clock::now())
{
}

float DeltaClock::elapsed() const
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float>(now - m_last_reset).count();
}

float DeltaClock::restart()
{
    auto now       = std::chrono::steady_clock::now();
    const float dt = std::chrono::duration<float>(now - m_last_reset).count();
    m_last_reset   = now;
    return dt;
}
} // namespace cs
