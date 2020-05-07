#pragma once

#include "entity/events.h"
#include "system.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <robin_hood.h>

namespace cs::system
{
/**
 * Keeps track of the entity ids in the diffrent registry
 */
struct EntityIds
{
    /** Entity id from the scenario scene registry */
    entt::entity global{entt::null};

    /** Entity id from the Relationship registry */
    entt::entity relationship{entt::null};
};

/**
 * Keeps the entity's name (if any), and it's set of IDs
 */
struct EntityNameAndIds
{
    /** ids for the global and relationship registry ids */
    EntityIds ids{};

    /** name for it's name, if no name it will default to the name components type */
    std::string name{};
};

/**
 * Just a pair of the mom and dad (Both Parents), to keep both name and their Ids
 */
struct ParentsName
{
    EntityNameAndIds mom{};
    EntityNameAndIds dad{};
};

class Relationship : public ISystem
{
private:
    /** A Table of relationships (To consider, make it a std::pair of uint8) */
    robin_hood::unordered_map<entt::entity, robin_hood::unordered_map<entt::entity, uint16_t>> m_rel_table{};

    /** A new registry of every spawned agent */
    entt::registry m_parents_reg{};

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
     * Adds the new entity to local registry, along with information of who its parents was
     *
     * This function triggers every time a new entity is born (made during runtime between two entities)
     *
     * @param event The event information that the function is subscribed to
     */
    void new_child_to_reg(const event::EntityBorn& event);

    /**
     * Handles deletion of the father, during the moms pregnancy
     *
     * Function is subscribed to the deletion event, and checks if there are any moms out there
     * that is pregnant and has the deletion event entity has a father, if so we add the father to
     * the local registry, using the add_to_reg function
     *
     * @param event The event information that the function is subscribed to
     */
    void delete_father(const event::EntityDeleted& event);

    /**
     * Adds a relationship table to the specified entity
     *
     * @note the table stores values in a 2 dimensional table where you can index it with uint16_t values
     * The first 8 bits represent friendship, the last 8 bits romantic intrest. However they do not <i>need</i>
     * to represent this, the users themself can decide what it should represent
     *
     * @param event The spawn event that triggers when a new entity is spawned
     */
    void add_relationship_table(const event::EntitySpawned& event);

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
     * Get the romantic value between e to other that goes from 0 to 255
     *
     * @param e Who's entity table you want to check for
     * @param other The other entity you want to index in the table
     * @return The value of e's romantic instrest to other
     */
    uint8_t get_romance(entt::entity e, entt::entity other);

    /**
     * Add amount to the romantic value that is there
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
     * @note Assumes entity ids are from the context registry and not the local relationship registry, unless specified otherwise
     * @param e The entity you want to get the parents of
     * @param is_local_ids True if the entity is from the local registry, false if it is from the global registry
     * @return The struct BothParentName that include the name, local- and global entity ids
     */
    ParentsName get_parents(entt::entity e, bool is_local_id = false);

    /**
     * Get the entity's children as a vector of EntityNameAndIds
     *
     * @note Assumes entity ids are from the context registry and not the local relationship registry, unless specified otherwise
     * @param e The entity you want to get the children of
     * @param is_local_id True if the entity is from the local registry, false if it is from the global registry
     * @return A vector of the children's name and ids
     */
    std::vector<EntityNameAndIds> get_children(entt::entity e, bool is_local_id = false);

    /**
     * Check if someone is related to you within n generations
     *
     * @note Assumes entity ids are from the context registry and not the local relationship registry, unless specified otherwise
     * @note Gen is generations, so if you set it to 1 it will find your parents and your children.
     * Set it to 2, and it will find your parents and children and their parents and children. Aka Siblings, grand parents and
     * grand-children. Set it to 3 and it will find your parents and children and their parents and children and their parent and
     * children, giving you aunts and uncles. Set it to 4 you will get cousin as well.
     *
     * @warning higher gen number exponentially increases compute time
     *
     * @param me The entity that is you
     * @param other The entity you want to see if you are related too
     * @param gen How distant family members you want to check (Default is 3 generations, see note)
     * @param is_local_ids True if the entity is from the local registry, false if it is from the global registry
     * @return True if the other entity is related to you within the set range, otherwise false
     */
    bool is_family(entt::entity me, entt::entity other, unsigned gen = 3, bool is_local_id = false);

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
