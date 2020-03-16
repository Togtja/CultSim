#pragma once
#include "memory.h"

#include <glm/vec3.hpp>

namespace cs::memory
{
class ResourceLocation : public IMemory
{
public:
    glm::vec3 m_location{};
    int m_number_of_entities{};

    ResourceLocation(ETag tags, glm::vec3 location, int number_of_entities) :
        IMemory(tags),
        m_location(location),
        m_number_of_entities(number_of_entities){};

    IMemory* clone() override
    {
        return new ResourceLocation(m_tags, m_location, m_number_of_entities);
    }
};
} // namespace cs::memory
