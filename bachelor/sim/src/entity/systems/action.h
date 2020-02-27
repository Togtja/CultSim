#pragma once
#include "entity/events.h"
#include "system.h"
#include "random_engine.h"

#include <entt/signal/dispatcher.hpp>

namespace cs::system
{
class Action : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;
};
} // namespace cs::system
