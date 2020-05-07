#pragma once

#include "system.h"

namespace cs::system
{
/**
 * The trait system handles assigning traits
 */
class Traits : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
