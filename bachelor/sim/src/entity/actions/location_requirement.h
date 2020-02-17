#pragma once
#include "requirement.h"
#include "entity/events.h"

#include <functional>
#include <string>

#include "entt/signal/dispatcher.hpp"
#include "glm/vec3.hpp"

namespace cs::action
{
class LocationRequirement : public IRequirement
{
private:
    glm::vec3 m_desired_pos{};
    entt::dispatcher& m_dispatcher{};

public:
    LocationRequirement(std::string name, glm::vec3 pos, entt::dispatcher& dispatcher) :
        IRequirement(name),
        m_desired_pos(pos),
        m_dispatcher(dispatcher){};
    ~LocationRequirement(){ m_dispatcher.sink<event::ArrivedAtDestination>().disconnect<&LocationRequirement::event_listener>(this);};

    void init() override;
    void event_listener(event::ArrivedAtDestination& event);
};
} // namespace cs::action
