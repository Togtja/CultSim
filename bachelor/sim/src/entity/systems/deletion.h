#pragma once
#include "entity/events.h"
#include "system.h"
namespace cs::system
{
class Deletion : public ISystem
{
public:
    using ISystem::ISystem;

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

    void check_and_delete(const event::DeleteEntity& event);
};
} // namespace cs::system
