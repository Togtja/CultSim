#pragma once

#include "system.h"

namespace cs::system
{
class Reproduction : public ISystem
{
private:
public:
    using ISystem::ISystem;
    void update(float dt) override;
};
} // namespace cs::system
