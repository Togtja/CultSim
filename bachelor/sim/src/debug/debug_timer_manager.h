#pragma once

#include <robin_hood.h>

namespace cs
{
class DebugTimerManager
{
private:
    friend DebugTimerManager& getDebugTimerResults();

    /** AutoTimer Results */
    robin_hood::unordered_map<std::string_view, double> m_results{};

    DebugTimerManager() = default;

public:
    DebugTimerManager& operator=(DebugTimerManager&&) = delete;
    DebugTimerManager& operator=(const DebugTimerManager&) = delete;
    DebugTimerManager(const DebugTimerManager&)            = delete;
    DebugTimerManager(DebugTimerManager&&)                 = delete;
    ~DebugTimerManager() noexcept                          = default;

    /**
     * Add a timer entry to the DebugTimerManager
     *
     * @param name Name of the timer result
     * @param time_ms Time it took in milliseconds
     */
    void add_entry(std::string_view name, double time_ms);

    /**
     * Show an ImGui debug UI with all active timers
     */
    void show_debug_ui() const;
};

/**
 * Get access to the only instance of a debug timer  result
 */
DebugTimerManager& getDebugTimerResults();

} // namespace cs
