#pragma once

#include "entity/events.h"
#include "system.h"

namespace cs::system
{
class Inventory : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
