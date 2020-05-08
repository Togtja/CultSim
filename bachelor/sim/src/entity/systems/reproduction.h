#pragma once

#include "entity/components/components.h"
#include "system.h"

#include <string>
#include <utility>

#include <entt/entity/registry.hpp>
#include <glm/vec3.hpp>

namespace cs::system
{
/**
 * System to handle reproduction of agent
 * It handle fertility and birth (spawning) of new agents (and inheriting parents abilities)
 */
class Reproduction : public ISystem
{
    /** A struct that stores the information for spawning a child */
    struct Child
    {
        /** The Type of the child to spawn (egg or parent type)*/
        std::string type{};

        /** The Position of where to spawn it */
        glm::vec3 position{};

        /** Boolean for it is an egg or nor */
        bool is_egg{false};

        /** What type the parent was, same as child, except if child is an egg */
        std::string parent_type{};

        /** Entity ids of who the parents was */
        component::Pregnancy::Parents parents{entt::null, entt::null};
    };

public:
    using ISystem::ISystem;

    /**
     * Run every frame and update their fertility, pregnancy status, and handle a pregnancy if is is about to occur
     *
     * @param dt The time between frames
     */
    void update(float dt) override;

    ISystem* clone() override;

private:
    /**
     * Hanldes the inheritice of traits from "from"
     *
     * @param traits The trait component you want to inherit to
     * @param from the entity/agent you want to inherit from
     */
    void inherit_traits(component::Traits& traits, entt::entity from);

    /** Updates all agent's fertility chances */
    void update_fertility();

    /**
     * Given a vector of the children struct spawn those children, and send an event
     *
     * @param children Vector of children
     */
    void spawn_children(const std::vector<Child>& children);
};
} // namespace cs::system
