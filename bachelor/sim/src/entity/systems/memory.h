#pragma once
#include "entity/events.h"
#include "system.h"

namespace cs::system
{
class Memory : public ISystem
{
private:
    float m_timer{};

    float m_max_memories{};

    float m_max_retention_time{};

public:
    using ISystem::ISystem;

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

    void update_memories(const event::CreatedMemory& event);
};
} // namespace cs::system
