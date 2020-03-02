#pragma once

#include <chrono>

namespace cs
{
/**
 * Keeps track of time between each call to restart, acts like a stop watch
 */
class DeltaClock
{
private:
    std::chrono::steady_clock::time_point m_last_reset;

public:
    DeltaClock();

    using TimeUnit = std::chrono::duration<float>;

    /**
     * Return the time since last restart
     *
     * @return Time in seconds since last restart
     */
    [[nodiscard]] float elapsed() const;
    /**
     * Restart the clock and get the time since last restart
     *
     * @return Time since last restart in seconds
     */
    [[nodiscard]] float restart();
};
} // namespace cs
