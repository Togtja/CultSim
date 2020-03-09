#pragma once
#include <entity/components/tags.h>

namespace cs::memory
{
class IMemory
{
public:
    IMemory(Etag tags) : m_tags(tags){};

    float m_time_since_creation{};
    Etag m_tags{};

    void update(float dt){m_time_since_creation += dt};

    virtual ~IMemory(){};
};
} // namespace cs::memory