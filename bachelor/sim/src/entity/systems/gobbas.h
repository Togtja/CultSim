#pragma once
#include "system.h"

namespace cs::system
{
/** TODO: Documentation */
class GOBBAS : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
