#pragma once
#include <chrono>
#include <string_view>

#include <robin_hood.h>
#include <spdlog/spdlog.h>

namespace cs
{
/* clang-format off */
#define _CS_CONCAT_(x, y) x##y
#define CS_CONCAT(x, y) _CS_CONCAT_(x, y)
#define CS_AUTOTIMER(name) const auto CS_CONCAT(cs_auto_timer_,__COUNTER__) = AutoTimer{#name}
/* clang-format on */

/**
 * Debug class used to automatically time how long it remained alive in a scope
 */
class AutoTimer
{
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
    using TimeUnit  = std::chrono::duration<double, std::milli>;

private:
    TimePoint m_start_time = std::chrono::steady_clock::now();
    std::string_view m_name{};

    /** AutoTimer Results */
    inline static robin_hood::unordered_map<std::string_view, double> s_results{};

public:
    explicit AutoTimer(std::string_view name) : m_name(name)
    {
    }

    AutoTimer& operator=(const AutoTimer&) = delete;
    AutoTimer& operator=(AutoTimer&&) = delete;
    AutoTimer(AutoTimer&&)            = delete;
    AutoTimer(const AutoTimer&)       = delete;

    ~AutoTimer() noexcept
    {
        TimeUnit timed    = std::chrono::steady_clock::now() - m_start_time;
        s_results[m_name] = timed.count();
    }

    /**
     * Show an ImGui debug UI with all active timers
     */
    static void show_debug_ui();
};
} // namespace cs
