#pragma once

#include "entity/events.h"
#include "entt/entity/registry.hpp"
#include "robin_hood.h"
#include "system.h"

#include <entt/signal/dispatcher.hpp>

namespace cs::system
{
struct ParentEntityIds
{
    entt::entity global_registry_id       = entt::null;
    entt::entity relationship_registry_id = entt::null;
};
struct ParentName
{
    ParentEntityIds ids{};
    std::string name{};
};
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
     * @note Assumes that the entity has both a relationship and name component
     */
    entt::entity add_to_reg(const entt::entity e);

    void new_child_to_reg(const event::BornEntity& event);

    void delete_father(const event::DeleteEntity& event);

    void add_relationship_table(entt::entity e);

    uint8_t get_friendship(entt::entity e, entt::entity other);

    void add_friendship(entt::entity e, entt::entity other, uint8_t amount);
    uint8_t get_romance(entt::entity e, entt::entity other);

    void add_romance(entt::entity e, entt::entity other, uint8_t amount);

    BothParentName get_parent(entt::entity e, bool is_local_ids = false);

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system