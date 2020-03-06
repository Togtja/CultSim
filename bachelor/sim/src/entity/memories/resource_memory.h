#pragma
#include "memory.h"
#include <glm/vec3.hpp>

namespace cs
{
class ResourceMemory : public IMemory
{
public:
    ResourceMemory(ETag tags, float time_of_creation, glm::vec3 location, int number_of_matching_entities) :
        IMemory(tags, time_of_creation),
        m_location(location),
        m_number_of_matching_entities(number_of_matching_entities){};

    glm::vec3 m_location{};

    int m_number_of_matching_entities{};

    
};
} // namespace cs