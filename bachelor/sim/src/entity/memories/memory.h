#pragma once

#include "entity/components/tags.h"

namespace cs::memory
{
/** TODO: Add Documentation */
struct IMemory
{
    ETag m_tags{};
    float m_time_since_creation{};

    IMemory(ETag tags) : m_tags(tags){};
    virtual ~IMemory() noexcept = default;

    void update(float dt)
    {
        m_time_since_creation += dt;
    };

    /**
     * Implement to create a clone of this memory
     *
     * @return A clone of this memory
     */
    virtual IMemory* clone() = 0;
};
} // namespace cs::memory
