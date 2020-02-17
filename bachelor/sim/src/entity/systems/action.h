#pragma once
#include "entity/events.h"
#include "system.h"

#include <entt/signal/dispatcher.hpp>

namespace cs::system
{
class Action : public ISystem
{
private:
    entt::dispatcher& m_dispatcher;

public:
    Action(entt::registry& registry, entt::dispatcher& dispatcher);

    ~Action() noexcept;

    void update(float dt) override;

private:
    void respond_arrive(const event::ArrivedAtDestination& data);
};
} // namespace cs::system
