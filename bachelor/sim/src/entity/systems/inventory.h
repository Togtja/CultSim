#pragma once

#include "entity/events.h"
#include "system.h"

namespace cs::system
{
class Inventory : public ISystem
{
public:
    Inventory(SystemContext context) : ISystem(context)
    {
        m_context.dispatcher->sink<event::DeleteEntity>().connect<&Inventory::drop_items>(this);
    }

    ~Inventory()
    {
        m_context.dispatcher->sink<event::DeleteEntity>().disconnect<&Inventory::drop_items>(this);
    }
    void update(float dt) override;

    void drop_items(const event::DeleteEntity& event);
};
} // namespace cs::system
