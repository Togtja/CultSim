#pragma once
#include "system.h"

#include <entt/signal/dispatcher.hpp>

namespace cs::system
{
class Movement : public ISystem
{
private:
    entt::dispatcher& m_dispatcher;

public:
    Movement(entt::registry& registry, entt::dispatcher& dispatcher);

    void update(float dt) override;
};
} // namespace cs::system
