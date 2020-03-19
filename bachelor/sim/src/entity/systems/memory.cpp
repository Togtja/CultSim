#include "memory.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/memories/resource_location.h"

#include <glm/glm.hpp>

namespace cs::system
{
void Memory::update(float dt)
{
    CS_AUTOTIMER(Memory System);

    m_timer += dt;
    auto& registry = m_context.registry;

    if (m_timer > 0.4f && m_timer < 0.6f)
    {
        auto view = registry->view<component::Memory, component::Vision>();
        view.each([dt, registry](entt::entity e, component::Memory& memory, const component::Vision& vision) {
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
                for (auto& memory : container.memory_storage)
                {
                    if (auto* res = dynamic_cast<memory::ResourceLocation*>(memory.get());
                        res && close_enough(res->m_location, location, 20.f))
                    {
                        res->m_number_of_entities = count;
                        duplicate                 = true;
                    }
                }
                if (!duplicate)
                {
                    container.memory_storage.push_back(std::unique_ptr<memory::ResourceLocation>(
                        new memory::ResourceLocation(ETag(TAG_Food | TAG_Location), location, count)));
                }
            }
        });
    }

    if (m_timer >= 1)
    {
        m_timer   = 0.f;
        auto view = registry->view<component::Memory>();

        view.each([dt, registry, this](entt::entity e, component::Memory& memory) {
            for (auto& memory_container : memory.memory_container)
            {
                for (auto& memory : memory_container.memory_storage)
                {
                    memory->update(1.f);
                }

                if (memory_container.memory_storage.size() != 0)
                {
                    // spdlog::get("agent")->warn("Size of memory storage {}: {}",
                    //                           tag_to_string(ETag(memory_container.memory_tags & ~TAG_Location)),
                    //                           memory_container.memory_storage.size());

                    if (dynamic_cast<memory::ResourceLocation*>(memory_container.memory_storage.front().get()))
                    {
                        auto pos = registry->get<component::Position>(e).position;
                        std::sort(
                            memory_container.memory_storage.begin(),
                            memory_container.memory_storage.end(),
                            [pos](const std::unique_ptr<memory::IMemory>& lhs, const std::unique_ptr<memory::IMemory>& rhs) {
                                auto res_lhs = dynamic_cast<memory::ResourceLocation*>(lhs.get());
                                auto res_rhs = dynamic_cast<memory::ResourceLocation*>(rhs.get());

                                float cost_lhs{};
                                float cost_rhs{};

                                cost_lhs = glm::distance(res_lhs->m_location, pos);
                                cost_rhs = glm::distance(res_rhs->m_location, pos);

                                cost_lhs -= res_lhs->m_number_of_entities * 10.f;
                                cost_rhs -= res_rhs->m_number_of_entities * 10.f;

                                return cost_lhs < cost_rhs;
                            });

            
                        // TODO: Remove magic number and put limit of memories remembered into Lua
                        while (memory_container.memory_storage.size() > m_max_memories || (memory_container.memory_storage.size() > 0 && memory_container.memory_storage.back()->m_time_since_creation <  m_max_retention_time))
                        {
                            memory_container.memory_storage.pop_back();
                        }
                    }
                }
            }
        });
    }
}

void Memory::update_memories(const event::CreatedMemory& event)
{
    auto memories  = m_context.registry->try_get<component::Memory>(event.entity);
    const auto pos = m_context.registry->try_get<component::Position>(event.entity);

    if (memories)
    {
        // Adds the new memory / Updates memories where applicable
        for (auto& memory_container : memories->memory_container)
        {
            // Memory creation / Updating
            if (memory_container.memory_tags & event.memory->m_tags == event.memory->m_tags)
            {
                auto duplicate = false;
                int i          = 0;
                for (auto& memory : memory_container.memory_storage)
                {
                    auto* res       = dynamic_cast<memory::ResourceLocation*>(memory.get());
                    auto* event_res = dynamic_cast<memory::ResourceLocation*>(event.memory.get());
                    if (res && close_enough(res->m_location, event_res->m_location, 20.f))
                    {
                        res->m_number_of_entities  = event_res->m_number_of_entities;
                        res->m_time_since_creation = 0.f;
                        duplicate                  = true;
                    }
                }
                // Create a new memory
                if (!duplicate)
                {
                    memory_container.memory_storage.emplace_back(event.memory->clone());
                }

                // Re-sort the memories
                if (event.memory->m_tags & TAG_Location)
                {
                    auto pos = m_context.registry->try_get<component::Position>(event.entity);
                    if (pos && dynamic_cast<memory::ResourceLocation*>(event.memory.get()))
                    {
                        std::sort(memory_container.memory_storage.begin(),
                                  memory_container.memory_storage.end(),
                                  [pos, this](const std::unique_ptr<memory::IMemory>& lhs,
                                              const std::unique_ptr<memory::IMemory>& rhs) {
                                      auto res_lhs = dynamic_cast<memory::ResourceLocation*>(lhs.get());
                                      auto res_rhs = dynamic_cast<memory::ResourceLocation*>(rhs.get());

                                      float cost_lhs{};
                                      float cost_rhs{};

                                      cost_lhs = glm::distance(res_lhs->m_location, pos->position);
                                      cost_rhs = glm::distance(res_rhs->m_location, pos->position);

                                      cost_lhs -= res_lhs->m_number_of_entities * 10.f;
                                      cost_rhs -= res_rhs->m_number_of_entities * 10.f;

                                      cost_lhs += res_lhs->m_time_since_creation;
                                      cost_rhs += res_rhs->m_time_since_creation;

                                      //Make sure that memories that have aged past retention_time are moved to the back of the list
                                      if (res_lhs->m_time_since_creation > m_max_retention_time)
                                      {
                                          cost_lhs += 1000.f;
                                      }
                                      if (res_rhs->m_time_since_creation > m_max_retention_time) 
                                      {
                                          cost_rhs += 1000.f;
                                      }
                                      
                                      return cost_lhs < cost_rhs;
                                  });
                    }
                }

                // Delete the most useless memories
                while (memory_container.memory_storage.size() >= m_max_memories ||
                       memory_container.memory_storage.back()->m_time_since_creation < m_max_retention_time)
                {
                    memory_container.memory_storage.pop_back();
                }
            }
        }
    }
}
} // namespace cs::system
