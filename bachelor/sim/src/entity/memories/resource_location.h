#pragma once
#include "memory.h"

#include <glm/vec3.hpp>

namespace cs::memory
{
/** TODO: Consider Variants if we have time */

/** ResourceLocations are subtypes of Memory which concern themselves with storing positions that the entity found resources in */
class ResourceLocation : public IMemory
{
public:
    /** The location at which the resource was discovered */
    glm::vec3 m_location{};

    /** How many other instances of the resource where found in the same area */
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
