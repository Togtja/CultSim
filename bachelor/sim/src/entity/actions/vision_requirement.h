#pragma once

#include "requirement.h"
#include "entity/events.h"
#include "entity/components/tags.h"

#include "entt/signal/dispatcher.hpp"
namespace cs::action
{
class VisionRequirement : public IRequirement
{
private:
    tags::ETag m_tags{}; 
    entt::dispatcher& m_dispatcher;

public:
    VisionRequirement(std::string name, entt::registry& registry, tags::ETag tags, entt::dispatcher& dispatcher) :
        IRequirement(std::move(name), registry),
        m_tags(tags),
        m_dispatcher(dispatcher){};
    ~VisionRequirement();

    void init(entt::entity entity) override;

    IRequirement* clone() override;
    void event_listener(const event::SensedEntity& event);
};
} // namespace cs::action