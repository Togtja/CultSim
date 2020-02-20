#pragma once

#include "system.h"
#include "random_engine.h"

namespace cs::system
{
class Requirement : public ISystem
{
private:
    RandomEngine m_rng{};

public:
    using ISystem::ISystem;

    void update(float dt) override;
};
} // namespace cs::system