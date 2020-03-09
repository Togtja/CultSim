#pragma once
#include "memory.h"

#include <glm/vec3.hpp>

namespace cs::memory
{
class ReourceLocation : public IMemory
{
public:
    ReourceLocation(ETag tags, glm::vec3 location, int number_of_entities) :
        IMemory(tags),
        m_location(location),
        m_number_of_enities(number_of_entities){};

    glm::vec3 m_location{};

    int m_number_of_enities{};

};
} // namespace cs::memory