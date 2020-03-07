#pragma once

#include "entity/components/tags.h"

namespace cs
{
class IMemory
{
public:
    IMemory(ETag tags, float time_of_creation) : m_tags(tags), m_time_of_creation(time_of_creation){};

    ETag m_tags{};
    float m_time_of_creation{};
};
} // namespace cs
