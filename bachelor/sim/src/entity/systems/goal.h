#pragma once
#include "system.h"

namespace cs::system
{
class Goal : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system