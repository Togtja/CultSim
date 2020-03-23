#pragma once

#include "entity/events.h"
#include "system.h"

namespace cs::system
{
class Inventory : public ISystem
{
public:
    using ISystem::ISystem;

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

    void drop_items(const event::DeleteEntity& event);
};
} // namespace cs::system
