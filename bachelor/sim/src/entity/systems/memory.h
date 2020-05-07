#pragma once
#include "entity/events.h"
#include "system.h"

namespace cs::system
{
/** TODO: Documentation */
class Memory : public ISystem
{
private:
    float m_timer{};

public:
    using ISystem::ISystem;

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

private:
    void update_memories(const event::CreatedMemory& event);
};
} // namespace cs::system
