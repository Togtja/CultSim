#pragma once

#include <vector>
#include <memory>
#include <variant>
#include <string_view>

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
     * Save the collected data to a file
     *
     * @param rpath The relative file path to save it
     * @param timestamp Set to true if the file name should be timestamped
     */
    void save_to_file(std::string_view rpath, bool timestamp = true);
};
} // namespace cs::debug
