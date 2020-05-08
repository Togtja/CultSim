#pragma once

#include "entity/events.h"
#include "system.h"

namespace cs::system
{
/**
 * The Inventory system serves to keep a creatures inventorys tags updated and to drop items in the inventory upon death
 *
 * The System will each frame check if the creature has an inventory, and set an inventory tag appropriately.
 * Additionally, it checks if the actual size of the inventory matches that stored in the inventory component, and update the
 * inventories tags with thw tags of it's contents if not, updating the size in the process.
 * Finally, it checks if the owning entity is about to be deleted, and removes any and all contents from the inventory, scattering
 * them around the creature before their death.
 */
class Inventory : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
