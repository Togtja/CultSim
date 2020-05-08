#pragma once
#include "entity/events.h"
#include "system.h"

namespace cs::system
{
/**
 * The Deletion system goes through all entities with the entt::tag Delete and deletes them
 *
 * The System deletes the entity in question, and sends out an event informing interested parties about the entity no longer being
 * valid.
 */

class Deletion : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
