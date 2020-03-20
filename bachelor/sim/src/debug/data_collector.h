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
