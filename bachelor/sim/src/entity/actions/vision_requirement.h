#pragma once

#include "requirement.h"
#include "entity/events.h"

#include <string>
#include <vector>

#include "entt/signal/dispatcher.hpp"
namespace cs::action
{
class VisionRequirement : public IRequirement
{
private:
    std::vector<std::string> m_tags{}; 
    entt::dispatcher& m_dispatcher;

public:
    VisionRequirement(std::string name, entt::registry& registry, std::vector<std::string> tags, entt::dispatcher& dispatcher) :
        IRequirement(std::move(name), registry),
        m_tags(tags),
        m_dispatcher(dispatcher){};
    ~VisionRequirement();

    void init(entt::entity entity) override;

    IRequirement* clone() override;
    void event_listener(const event::SensedEntity& event);
};
} // namespace cs::action