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
    /** Keeps track of the last time we reset this DeltaClock instance */
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

    /**
     * Restart the clock and get the time since last restart
     *
     * @return Time since last restart in the time unit of the clock
     */
    [[nodiscard]] TimeUnit restart_time_unit();
};
} // namespace cs
