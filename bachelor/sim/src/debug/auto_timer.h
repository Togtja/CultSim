#pragma once
#include <chrono>
#include <string_view>

#include <robin_hood.h>
#include <spdlog/spdlog.h>
#include <robin_hood.h>

namespace cs
{
/* clang-format off */
#define _CS_CONCAT_(x, y) x##y
#define CS_CONCAT(x, y) _CS_CONCAT_(x, y)
#define CS_AUTOTIMER(name) const auto CS_CONCAT(cs_auto_timer_,__COUNTER__) = AutoTimer{#name}
#define CS_AUTOTIMER_EXPRNAME(name) const auto CS_CONCAT(cs_auto_timer_,__COUNTER__) = AutoTimer{name}
/* clang-format on */

/**
 * Debug class used to automatically time how long it remained alive in a scope
 */
class AutoTimer
{
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
    using TimeUnit  = std::chrono::duration<double, std::milli>;

    struct Record
    {
        std::string_view name{};
        double time_ms{};
    };

private:
    TimePoint m_start_time = std::chrono::steady_clock::now();
    std::string_view m_name{};

    /** AutoTimer Results */
    inline static std::vector<Record> s_results{};

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
        TimeUnit timed = std::chrono::steady_clock::now() - m_start_time;

        auto itr = std::find_if(s_results.begin(), s_results.end(), [this](const auto& r) { return r.name == m_name; });

        if (itr == s_results.end())
        {
            s_results.push_back({m_name, timed.count()});
        }
        else
        {
            itr->time_ms += timed.count();
        }
    }

    /**
     * Show an ImGui debug UI with all active timers
     */
    static void show_debug_ui();
};

/**
 * Measure the execution time of a single callable
 *
 * @param func The callable to measure
 * @param args The arguments to pass to the callable
 * @tparam Callable The type of the function
 * @tparam Args The argument types for the function
 * @return The execution time in milliseconds
 */
template<typename Callable, typename... Args, typename = std::enable_if_t<std::is_invocable_v<Callable, Args...>>>
double measure_callable(Callable&& func, Args&&... args)
{
    auto before = std::chrono::steady_clock::now();
    std::invoke(func, std::forward<Args>(args)...);
    auto after = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(after - before).count();
}
} // namespace cs
