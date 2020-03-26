#pragma once
#include "entity/events.h"
#include "system.h"
namespace cs::system
{
class Deletion : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
