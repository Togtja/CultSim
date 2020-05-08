#include "memory.h"
#include "common_helpers.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/memories/resource_location.h"

#include <glm/glm.hpp>
#include <taskflow/taskflow.hpp>

namespace cs::system
{
void Memory::initialize()
{
    m_context.dispatcher->sink<event::CreatedMemory>().connect<&Memory::update_memories>(*this);
}

void Memory::deinitialize()
{
    m_context.dispatcher->sink<event::CreatedMemory>().disconnect<&Memory::update_memories>(*this);
}

/** TODO: Move stuff into functions */
void Memory::update(float dt)
{
    CS_AUTOTIMER(Memory System);

    m_timer += dt;
    auto& registry = m_context.registry;

    /** Makes memories while we are moving around */
    if (m_timer > 0.468f && m_timer < 0.532f)
    {
        auto view = registry->view<component::Memory, component::Vision>();
        view.each([this, registry](entt::entity e, component::Memory& memory, const component::Vision& vision) {
            for (auto& container : memory.memory_container)
            {
                int count{};
                auto location  = registry->get<component::Position>(e).position;
                bool duplicate = false;

                for (auto entity : vision.seen)
                {
                    if (registry->valid(entity))
                    {
                        auto entity_tags = registry->try_get<component::Tags>(entity);
                        if (entity_tags && entity_tags->tags & container.memory_tags)
                        {
                            count++;
                        }
                    }
                }
                auto memory = std::unique_ptr<memory::ResourceLocation>(
                    new memory::ResourceLocation(ETag(TAG_Food | TAG_Location), location, count));
                add_memory(e, memory.get(), container);
            }
        });
    }

    if (m_timer >= 1.f)
    {
        auto view = registry->view<component::Memory>();

        tf::Taskflow taskflow{};

        taskflow.parallel_for(view.begin(), view.end(), [registry, this, &view](entt::entity e) {
            auto& memory = view.get(e);

            for (auto& memory_container : memory.memory_container)
            {
                for (auto& memory : memory_container.memory_storage)
                {
                    memory->update(m_timer);
                }

                if (memory_container.memory_storage.size() != 0)
                {
                    sort_memories(e, memory_container);

                    while (memory_container.memory_storage.size() > memory.max_memories ||
                           (memory_container.memory_storage.size() > 0 &&
                            memory_container.memory_storage.back()->m_time_since_creation > memory.max_retention_time))
                    {
                        memory_container.memory_storage.pop_back();
                    }
                }
            }
        });

        m_context.executor->run(taskflow).get();
        m_timer = 0.f;
    }
}

ISystem* Memory::clone()
{
    return new Memory(m_context);
}

void Memory::add_memory(entt::entity e, memory::IMemory* memory, memory::Container& container)
{
    auto duplicate = false;
    for (auto& old_memory : container.memory_storage)
    {
        auto* o_res = dynamic_cast<memory::ResourceLocation*>(old_memory.get());
        auto* n_res = dynamic_cast<memory::ResourceLocation*>(memory);
        if (o_res && close_enough(o_res->m_location, n_res->m_location, 20.f))
        {
            o_res->m_number_of_entities  = n_res->m_number_of_entities;
            o_res->m_time_since_creation = 0.f;
            duplicate                    = true;
        }
    }
    /** Create a new memory */
    if (!duplicate)
    {
        container.memory_storage.emplace_back(memory->clone());
    }
}

void Memory::sort_memories(entt::entity e, memory::Container& container)
{
    if (dynamic_cast<memory::ResourceLocation*>(container.memory_storage.front().get()))
    {
        const auto pos                   = m_context.registry->get<component::Position>(e).position;
        const auto memory_retention_time = m_context.registry->get<component::Memory>(e).max_retention_time;

        std::sort(container.memory_storage.begin(),
                  container.memory_storage.end(),
                  [pos, memory_retention_time](const std::unique_ptr<memory::IMemory>& lhs,
                                               const std::unique_ptr<memory::IMemory>& rhs) {
                      auto res_lhs = dynamic_cast<memory::ResourceLocation*>(lhs.get());
                      auto res_rhs = dynamic_cast<memory::ResourceLocation*>(rhs.get());

                      float cost_lhs{};
                      float cost_rhs{};

                      cost_lhs = glm::distance(res_lhs->m_location, pos);
                      cost_rhs = glm::distance(res_rhs->m_location, pos);

                      cost_lhs -= res_lhs->m_number_of_entities * 10.f;
                      cost_rhs -= res_rhs->m_number_of_entities * 10.f;
                      cost_lhs += res_lhs->m_time_since_creation;
                      cost_rhs += res_rhs->m_time_since_creation;

                      /** Make sure that memories that have aged past retention_time are moved to the back of the
                       list */
                      if (res_lhs->m_time_since_creation > memory_retention_time)
                      {
                          cost_lhs += 1000.f;
                      }

                      if (res_rhs->m_time_since_creation > memory_retention_time)
                      {
                          cost_rhs += 1000.f;
                      }

                      /**Make sure entities that have no entities are rated a lot lower than others */
                      if (res_lhs->m_number_of_entities == 0)
                      {
                          cost_lhs += 1000.f;
                      }

                      if (res_rhs->m_number_of_entities == 0)
                      {
                          cost_rhs += 1000.f;
                      }

                      return cost_lhs < cost_rhs;
                  });
    }
}

void Memory::update_memories(const event::CreatedMemory& event)
{
    auto memories = m_context.registry->try_get<component::Memory>(event.entity);

    if (memories)
    {
        /** Adds the new memory / Updates memories where applicable */
        for (auto& memory_container : memories->memory_container)
        {
            /** Memory creation / Updating */
            if ((memory_container.memory_tags & event.memory->m_tags) == event.memory->m_tags)
            {
                add_memory(event.entity, event.memory.get(), memory_container);
            }
        }
    }
}
} // namespace cs::system
