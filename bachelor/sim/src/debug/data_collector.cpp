#include "data_collector.h"
#include "debug/auto_timer.h"
#include "filesystem/filesystem.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include <gfx/ImGUI/imgui.h>
#include <gfx/ImGUI/imgui_plot.h>
#include <spdlog/fmt/fmt.h>

namespace cs::debug
{
void DataCollector::add_collector(std::unique_ptr<DataCollector::Command> new_collector)
{
    m_collectors.push_back(std::move(new_collector));
    m_samples.emplace_back();
}

void DataCollector::sample()
{
    for (unsigned i = 0u; i < m_collectors.size(); ++i)
    {
        m_samples[i].push_back(m_collectors[i]->execute());
    }
}

void DataCollector::set_sampling_rate(float seconds)
{
    m_sampling_rate = seconds;
}

void DataCollector::update(float dt)
{
    CS_AUTOTIMER(Data Sampling);

    m_time_since_sample += dt;
    if (m_time_since_sample > m_sampling_rate)
    {
        m_time_since_sample = 0.f;
        sample();
    }
}

void DataCollector::clear()
{
    m_collectors.clear();
    m_samples.clear();
}

void DataCollector::show_ui()
{
    /** TODO: Make it more elaborate */
    static int data_offset = 100;
    ImGui::DragInt("Data (display) size", &data_offset, 1.f, 10, 1000);
    if (data_offset < 10)
    {
        data_offset = 10;
    }

    if (ImGui::TreeNode("Data Collection"))
    {
        for (unsigned i = 0u; i < m_collectors.size(); ++i)
        {
            ImGui::PlotConfig conf;
            int offset = 0;
            int count  = m_samples[i].size();

            // Set offset based on slider
            if (m_samples[i].size() >= data_offset)
            {
                offset = m_samples[i].size() - data_offset;
                count  = data_offset;
            }
            conf.values.offset = offset;
            conf.values.ys     = m_samples[i].data();
            conf.values.count  = count;

            conf.scale.min = *std::min_element(m_samples[i].begin(), m_samples[i].end());
            conf.scale.max = *std::max_element(m_samples[i].begin(), m_samples[i].end());

            auto overlay_cstr = fmt::format("{}: {}", m_collectors[i]->get_name(), m_samples[i].back());
            conf.overlay_text = overlay_cstr.c_str();

            conf.tooltip.show   = true;
            auto tooltip_cstr   = fmt::format("data point: %g\n{}: %8.4g", m_collectors[i]->get_name());
            conf.tooltip.format = tooltip_cstr.c_str();

            conf.frame_size     = ImVec2(300, 75);
            conf.line_thickness = 2.f;
            ImGui::Plot(fmt::format("##{}_{}", m_collectors[i]->get_name(), i).c_str(), conf);
        }

        ImGui::TreePop();
    }
}

void DataCollector::save_to_file(std::string_view rpath, bool timestamp)
{
    std::ostringstream stream{};

    /** Header row */
    stream << "Collector,";
    for (unsigned i = 0u; i < m_samples.front().size(); ++i)
    {
        stream << m_sampling_rate * i << ',';
    }
    stream << '\n';

    /** Sampler data */
    for (unsigned i = 0u; i < m_collectors.size(); ++i)
    {
        stream << m_collectors[i]->get_name() << ',';
        for (const auto& sample : m_samples[i])
        {
            stream << sample << ',';
        }
        stream << '\n';
    }

    /** Create path */
    std::string final_output_path;
    std::transform(rpath.cbegin(), rpath.cend(), std::back_inserter(final_output_path), [](char c) {
        return std::isspace(c) ? '_' : std::tolower(c);
    });

    if (timestamp)
    {
        char timebuf[128];
        const auto now     = std::chrono::system_clock::now();
        std::time_t time_t = std::chrono::system_clock::to_time_t(now);
        std::strftime(timebuf, 128, "%d%m%y_%H%M%S", std::localtime(&time_t));

        final_output_path += fmt::format("_{}", timebuf);
    }

    final_output_path += ".csv";

    fs::write_file(final_output_path, stream.str());
}

} // namespace cs::debug
