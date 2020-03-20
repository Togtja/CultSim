#include "data_collector.h"

namespace cs::debug
{
void DataCollector::sample()
{
    for (const auto& sampler : m_collectors)
    {
        sampler->execute();
    }
}

void DataCollector::save_to_file(std::string_view rpath, bool timestamp)
{
}

} // namespace cs::debug
