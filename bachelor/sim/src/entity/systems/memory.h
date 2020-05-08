#pragma once
#include "entity/events.h"
#include "entity/memories/container.h"
#include "entity/memories/memory.h"
#include "system.h"

#include <entt/entity/registry.hpp>

namespace cs::system
{
/**
 * The Memory system creates new memories, keeps the list of memories sorted, and deletes old irrelevant memories
 *
 * The System runs two operations, the creation and sorting & deletion of memories. These systems run delayed from eachother by
 * half a second, meaning that both systems run about once a second. The Memory creation records all types of entities currently
 * visible to the entity at that point in time and checks if another memory exists within a short distance from the entities
 * current location. If so, that memory is updated with the new count of entities. Otherwise, a new memory is created.
 * Each type of entity, differentiated through their tags, gets their own type of memories, meaning that food and water will have
 * different memories associated with them.
 * The memory sorting ensures that the most relevant memory at that point in time, based on memory age, distance to target
 * position and number of entities seen. If the amount of memories exceeds a predefined maximum, memories at the back of the list
 * are pruned away. Additionally the system removes memories that have 0 entities seen.
 */
class Memory : public ISystem
{
private:
    float m_timer{};

public:
    using ISystem::ISystem;

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

private:
    /**
     * add_memory checks the distance between the memory and existing memories, updating the old memories or creating a new one
     *
     * The function goes through every memory in the provided container and checks the distance between it and the memory sent in.
     * If there is a memory found that is within a certain distance from this new one, that memory is updated by the data from the
     * new memory.
     *
     * @param e The entity creating the memory
     * @param memory The memory being created
     * @param container The memory_container the memory is being created into
     */
    void add_memory(entt::entity e, memory::IMemory* memory, memory::Container& container);

    /**
     * sort_memories goes through a container and sorts the memories inside it, putting the most desireable memories in the front
     *
     * @param e The entity that owns the container
     * @param container The container in question
     */
    void sort_memories(entt::entity e, memory::Container& container);

    /**
     * update_memories is triggered by outside systems to add new memories
     *
     * When the system receives a CreatedMemory event it will use the memory stored in the event tho either create a new memory or
     * update an existing one, based on how far away from the location of other memories we are.
     *
     * @param event The event that triggers the function
     */
    void update_memories(const event::CreatedMemory& event);
};
} // namespace cs::system
