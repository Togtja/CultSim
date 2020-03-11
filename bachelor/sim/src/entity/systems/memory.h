#pragma once
#include "system.h"

namespace cs::system
{
class Memory : public ISystem
{
private:
    float m_timer{};

public:
    using ISystem::ISystem;

    void update(float dt) override;
};
} // namespace cs::system
