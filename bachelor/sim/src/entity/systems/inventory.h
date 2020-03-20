#pragma once

#include "entity/events.h"
#include "system.h"

namespace cs::system
{
class Inventory : public ISystem
{
public:
    void initialize() override;

    void deinitialize() override;

    Inventory(SystemContext context) : ISystem(context)
    {
    }

    void update(float dt) override;

    void drop_items(const event::DeleteEntity& event);
};
} // namespace cs::system
