#pragma once
#include "system.h"
#include "entity/events.h"
namespace cs::system
{
class Memory : public ISystem
{
private:
    float m_timer{};

    float m_max_memories{};

    float m_max_retention_time{};

public:
    Memory(SystemContext context) : ISystem(context)
    {
        context.dispatcher->sink<event::CreatedMemory>().connect<&Memory::update_memories>(this);
    }

    ~Memory()
    {
        m_context.dispatcher->sink<event::CreatedMemory>().disconnect<&Memory::update_memories>(this);
    }
    void update(float dt) override;

    void update_memories(const event::CreatedMemory& event);
};
} // namespace cs::system
