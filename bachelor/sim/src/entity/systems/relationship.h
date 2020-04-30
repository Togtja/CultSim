#pragma once

#include "entity/events.h"
#include "entt/entity/registry.hpp"
#include "robin_hood.h"
#include "system.h"

#include <entt/signal/dispatcher.hpp>

namespace cs::system
{
/**
 * ParentEntityIds keeps track on their ids in the diffrent registry
 * global_registry_id for the m_context.registry id
 * and relationship_registry_id for the local m_parents_reg
 */
struct ParentEntityIds
{
    entt::entity global_registry_id       = entt::null;
    entt::entity relationship_registry_id = entt::null;
};

/**
 * ParentName keeps the parent's name (if any), and it's set of IDs
 * ids for the global and relationship registry ids
 * name for it's name, if no name it will default to type
 */
struct ParentName
{
    ParentEntityIds ids{};
    std::string name{};
};

/**
 * Just a pair of the mom and dad (Both Parents), to keep both name and their Ids
 */
struct BothParentName
{
    ParentName mom{};
    ParentName dad{};
};
class Relationship : public ISystem
{
private:
    /** A Table of relationship (To consider, make it a std::pair of uint8) */
    robin_hood::unordered_map<entt::entity, robin_hood::unordered_map<entt::entity, uint16_t>> m_rel_table;

    /** A new registry of every spawned agent */
    entt::registry m_parents_reg;

public:
    using ISystem::ISystem;

    Relationship(Relationship&&) = default;

    Relationship& operator=(Relationship&&) = default;

    Relationship(const Relationship& other);

    Relationship& operator=(const Relationship& other);

    void initialize() override;

    void deinitialize() override;

    /**
     * Add the entity to the local registry
     *
     * @note Assumes that the entity has both a relationship and name component
     * @param e The entity global registry id
     * @return The entity local registry id
     */
    entt::entity add_to_reg(const entt::entity e);

    /**
     * Adds the new entity to local registry, along with information of who its parents was.
     *
     * This function triggers every time a new entity is born (made during runtime between two entities)
     *
     * @param event The event information that the function is subscribed to
     */
    void new_child_to_reg(const event::BornEntity& event);

    /**
     * Handles deletion of the father, during the moms pregnancy
     *
     * Function is subscribed to the deletion event, and checks if there are any moms out there
     * that is pregnant and has the deletion event entity has a father, if so we add the father to
     * the local registry, using the add_to_reg function
     *
     * @param event The event information that the function is subscribed to
     */
    void delete_father(const event::DeleteEntity& event);

    /** Adds a relationship table to the specified entity
     *
     * @note the table stores values in a 2 dimensional table where you can index it with uint16_t values
     * The first 8 bits represent firendship, the last 6 bits romatic intrest. However they do not _need_
     * to represent this, the uses themself can decied what it should represent
     *
     * @param e entity you want to add a relationship table to
     */
    void add_relationship_table(entt::entity e);

    /**
     * Get the friendship value between e to other that goes from 0 to 255
     *
     * @param e Who's entity table you want to check for
     * @param other The other entity you want to index in the table
     * @return The value of e's friendship to other
     */
    uint8_t get_friendship(entt::entity e, entt::entity other);

    /**
     * Add amount to the friendship value that is there
     *
     * @note To remove just "add" a negative number
     * @param e Who's entity table you want to add for
     * @param other The other entity you want to index in the table
     * @param amount The amount you want to add
     */
    void add_friendship(entt::entity e, entt::entity other, uint8_t amount);

    /**
     * Get the romatic value between e to other that goes from 0 to 255
     *
     * @param e Who's entity table you want to check for
     * @param other The other entity you want to index in the table
     * @return The value of e's romatic instrest to other
     */
    uint8_t get_romance(entt::entity e, entt::entity other);

    /**
     * Add amount to the romatic value that is there
     *
     * @note To remove just "add" a negative number
     * @param e Who's entity table you want to add for
     * @param other The other entity you want to index in the table
     * @param amount The amount you want to add
     */
    void add_romance(entt::entity e, entt::entity other, uint8_t amount);

    /**
     * Get the name and ids of an entity's parents
     *
     * @param e The entity you want to get the parents of
     * @param is_local_ids True if the entity is from the local registry. False if it is from the global registry
     * @return The struct BothParentName that include the name, local- and global entity ids
     */
    BothParentName get_parent(entt::entity e, bool is_local_ids = false);

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system