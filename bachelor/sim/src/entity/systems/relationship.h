#pragma once

#include "entt/entity/registry.hpp"
#include "robin_hood.h"
#include "system.h"

namespace cs::system
{
class Relationship : public ISystem
{
private:
    // A Table of relationship (To consider, make it a std::pair of uint8)
    robin_hood::unordered_map<entt::entity, robin_hood::unordered_map<entt::entity, uint16_t>> rel_table;

public:
    using ISystem::ISystem;

    void initialize() override;

    void add_agent(entt::entity me);
    /** Get what "me" feel about the other agent */
    uint8_t get_friendship(entt::entity me, entt::entity other);

    void add_friendship(entt::entity me, entt::entity other, uint8_t amount);

    /** Get what "me" feel about the other agent */
    uint8_t get_romance(entt::entity me, entt::entity other);

    /** */
    void add_romance(entt::entity me, entt::entity other, uint8_t amount);

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system