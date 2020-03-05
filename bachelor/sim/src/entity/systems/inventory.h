#pragma once

#include "system.h"

namespace cs::system
{
class Inventory : public ISystem
{
public:
    using ISystem::ISystem;
    void update(float dt) override;
};
} // namespace cs::system