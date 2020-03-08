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

    void update(float dt) override{};

    static ResourceMemory sort(ResourceMemory const& lhs, ResourceMemory const& rhs, entt::entity e, entt::registry& r)
    {
        float cost_lhs{};
        float cost_rhs{};

        if (r.valid(e))
        {
            auto pos = r.get<component::Position>(e);
            cost_lhs += glm::distance(pos.position, lhs.m_location);
            cost_rhs += glm::distance(pos.position, rhs.m_location);

            cost_lhs -= 10 * lhs.m_number_of_matching_entities;
            cost_rhs -= 10 * rhs.m_number_of_matching_entities;
            if (cost_lhs > cost_rhs) 
            {
                return rhs;
            }
        }
            return lhs;
    }
};
} // namespace cs