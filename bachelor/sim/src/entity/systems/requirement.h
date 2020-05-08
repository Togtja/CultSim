#pragma once

#include "entity/events.h"
#include "system.h"

namespace cs::system
{
/**
 * The Requirement system operates on systems that have a requirement component to fullfill that requirement
 *
 * The Requirement system checks for entities that have an associated requirement component
 * Based on the components tags, it then begins to work to fullfill said requirement, removing the component once it is done
 * The types of requirements currently supported are:
 * Location requirements, which make the entity move to a certain position
 * Vision requirements, which require the entity to see another entity with certain tags
 * Find requirements, which make the entity search for an entity with certain tags, wandering randomly until they find one
 * Tag requirements, which add a new tag to the entity
 * Inventory requirement, which checks to see if we have an entity with a specific tag in our inventory
 */
class Requirement : public ISystem
{
public:
    using ISystem::ISystem;

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

    void remove_requirements(const event::EntityDeleted& event);

private:
    /**
     * goto_location goes through all entities with a Locationrequirment component and moves them to their target location
     *
     * The function uses the pathfinding algorythm to make the entity in question move towards a target location stored in the
     * LocationRequirement component. It then checks every frame if the distance to the target has decreased. If it does not for a
     * certain amount of time the requirement fails
     *
     * @param dt The time since the last frame
     */
    void goto_location(float dt);

    /**
     * look_for_target goes through all entities with a visionrequirement component and checks if they can see their targets
     *
     * The function goes through all entities seen by the entity in question and checks if it matches the tags of the requirement.
     * If it finds no such entity, the requirement fails.
     *
     * @param dt The time since the last frame
     */
    void look_for_target(float);

    /**
     * find_target goes through all entities with a findrequirement component and makes them find their target
     *
     * The function goes through all entities seen by the entity in question and checks if they match the tags of the requirement.
     * If it finds no such entity, it uses the memory component (if applicable) to go to a location it remembers it's target
     * being. If that does not find the target, it moves randomly around the map. If it does find it's target it adds a
     * locationrequirement with the targets position as it's target and adds the target as the target of the entities current
     * action.
     * If the entity takes to long to find it's target the requirement fails.
     *
     * @param dt The time since the last frame
     */
    void find_target(float dt);

    /**
     * add_tag goes through all entities with a tagrequirement component and makes them add the requirements tags to their own
     *
     * @param dt The time since the last frame
     */
    void add_tag(float dt);

    /**
     * check_inventory goes through all entities with a inventoryrequirement component and checks their inventories for their
     * target
     *
     * The function checks first if the inventories tags contain the tags of our target. Then it checks through each entity in the
     * inventory to see if it has the tags we are looking for. If it finds an appropriate entity it sets the current actions
     * target to that entity and removes the Inventory requirement component.
     * If there is no such entity, the requirement fails
     *
     * @param dt The time since the last frame
     */
    void check_inventory(float dt);
};
} // namespace cs::system
