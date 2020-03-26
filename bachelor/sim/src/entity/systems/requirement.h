#pragma once

#include "entity/events.h"
#include "random_engine.h"
#include "system.h"

namespace cs::system
{
/**
 * The Requirement system operates on systems that have a requirement component to fullfill that requirement
 *
 * The Requirement system checks for entities that have an associated requirement component
 * Based on the components tags, it then begins to work to fullfill said requirement, removing the component once it is done
 * The types of requirements currently supportet are:
 * Location requirements, which make the entity move to a certain position
 * Vision requirements, which require the entity to see another entity with certain tags
 * Find requirements, which make the entity search for an entity with certain tags, wandering randomly until they find one
 * Tag requirements, which add a new tag to the entity
 */
class Requirement : public ISystem
{
public:
    using ISystem::ISystem;

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

    void remove_requirements(const event::DeleteEntity& event);
};
} // namespace cs::system
