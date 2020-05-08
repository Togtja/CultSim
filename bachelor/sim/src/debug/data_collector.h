#pragma once

#include <memory>
#include <string_view>
#include <variant>
#include <vector>

#include <glm/vec2.hpp>

namespace cs::debug
{
class DataCollector
{
public:
    /**
     * Data collection command that can be overridden
     */
    struct Command
    {
        virtual ~Command() noexcept         = default;
        virtual float execute()             = 0;
        virtual std::string_view get_name() = 0;
    };

private:
    /** Data collectors */
    std::vector<std::unique_ptr<Command>> m_collectors{};

    /** Samples */
    std::vector<std::vector<float>> m_samples{};

    /** How often we sample */
    float m_sampling_rate = {0.f};

    /** Time since sample */
    float m_time_since_sample{0.f};

public:
    DataCollector() = default;

    /**
     * Add an existing collector to the collection vector
     *
     * @param new_collector The new collector that we will take ownership of
     */
    void add_collector(std::unique_ptr<DataCollector::Command> new_collector);

    /**
     * Construct a new collector directly into the data collector
     *
     * @tparam T Type of the Data Collector Command to construct
     * @tparam Args Arguments required to construct a T
     * @param args The arguments needed to construct a T
     */
    template<typename T,
             typename... Args,
             typename = std::enable_if_t<std::is_base_of_v<DataCollector::Command, std::decay_t<T>>>>
    void add_collector(Args&&... args)
    {
        m_collectors.emplace_back(new T{std::forward<Args>(args)...});
        m_samples.push_back({});
    }

    /**
     * Run data collection commands and sample all data
     */
    void sample();

    /**
     * Set the sampling rate for the data collector
     *
     * @param seconds The number of seconds between each sample
     */
    void set_sampling_rate(float seconds);

    /**
     * Update the data collector, it will sample if required
     *
     * @param dt The delta time since last update
     */
    void update(float dt);

    /** Clear all data samples and collectors */
    void clear();

    /**
     * Show data collector UI
     */
    void show_ui();

    /**
     * Save the collected data to a csv file
     *
     * @param rpath The relative file path to save it <i>without</i> any suffix like .csv
     * @param timestamp Set to true if the file name should be timestamped
     */
    void save_to_file(std::string_view rpath, bool timestamp = true);
};
} // namespace cs::debug
