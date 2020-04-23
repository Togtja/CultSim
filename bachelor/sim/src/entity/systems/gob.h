#pragma once
#include "system.h"

namespace cs::system
{
class GOB : public ISystem
{
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
