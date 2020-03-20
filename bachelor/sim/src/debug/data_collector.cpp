#include "data_collector.h"

namespace cs::debug
{
void DataCollector::add_collector(std::unique_ptr<DataCollector::Command> new_collector)
{
    m_collectors.push_back(std::move(new_collector));
    m_samples.push_back({});
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
    m_time_since_sample += dt;
    if (m_time_since_sample > m_sampling_rate)
    {
        sample();
    }
}

void DataCollector::clear()
{
    m_collectors.clear();
    m_samples.clear();
}

void DataCollector::save_to_file(std::string_view rpath, bool timestamp)
{
}

} // namespace cs::debug
