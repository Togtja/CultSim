#pragma once

#include "entity/events.h"
#include "system.h"

namespace cs::system
{
/**
 * System that enforces a limit to the amount of children an entity may have
 *
 * The Reproduction system handles the background cleanup for reproduction started in the reproduction action
 * The System counts the number of children an entity has had, and once that is reached, removes the need and component associated
 *with reproduction
 *
 * Will likely handle pregnancy and similar things associated with reproduction
 *
 *@Note Despite it's name, the reproduction system is not required for reproduction to function at the moment, only for entities
 *to eventually stop reproducing. Reproduction is handled by the corresponding action
 */
class Reproduction : public ISystem
{
public:
    using ISystem::ISystem;
    
    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    void delete_father(const event::DeleteEntity& event);
};
} // namespace cs::system
