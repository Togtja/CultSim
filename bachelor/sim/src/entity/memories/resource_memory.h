#pragma
#include "memory.h"

#include <glm/vec3.hpp>

namespace cs
{
class ResourceMemory : public IMemory
{
public:
    ResourceMemory(Etag tag, glm::vec3 pos, int count, float time) :
        IMemory(tag),
        location(pos),
        number_of_matching_entities(count),
        time_of_creation(time){};

    glm::vec3 location{};

    int number_of_matching_entities{};

    float time_of_creation{};
    
};
} // namespace cs