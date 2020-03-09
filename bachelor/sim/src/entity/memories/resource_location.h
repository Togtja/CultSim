#pragma once
#include "memory.h"

#include <glm/vec3.hpp>

namespace cs::memory
{
class ResourceLocation : public IMemory
{
public:
    ResourceLocation(ETag tags, glm::vec3 location, int number_of_entities) :
        IMemory(tags),
        m_location(location),
        m_number_of_entities(number_of_entities){};

    glm::vec3 m_location{};

    int m_number_of_entities{};
};
} // namespace cs::memory
