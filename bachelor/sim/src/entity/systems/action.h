#pragma once
#include "entity/events.h"
#include "system.h"

#include <entt/signal/dispatcher.hpp>

namespace cs::system
{
class Action : public ISystem
{
private:

public:
    using ISystem::ISystem;

    void update(float dt) override;

};
} // namespace cs::system
