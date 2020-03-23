#pragma once
#include "system.h"

#include <entt/signal/dispatcher.hpp>

namespace cs::system
{
class Movement : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
