#include "memory.h"
#include "common_helpers.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/memories/resource_location.h"
#include "gfx/renderer.h"

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
                        if (entity_tags &&
                            ((entity_tags->tags & container.memory_tags) == ETag(container.memory_tags & (~ETag::TAG_Location))))
                        {
                            count++;
                        }
                    }
                }
                if (count != 0)
                {
                    auto memory = std::unique_ptr<memory::ResourceLocation>(
                        new memory::ResourceLocation(ETag(container.memory_tags | TAG_Location), location, count));
                    add_memory(e, memory.get(), container);
                }
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

void Memory::update_imgui()
{
    static bool draw_memories = false;
    if (ImGui::TreeNode("Memories"))
    {
        ImGui::Checkbox("Draw Memory Locations", &draw_memories);
        ImGui::TreePop();
    }

    if (draw_memories)
    {
        auto view = m_context.registry->view<component::Memory, component::Vision>();
        view.each([this](const component::Memory& memories, const component::Vision& vision) {
            auto max = memories.memory_container.size();
            for (int i = 0; i < memories.memory_container.size(); i++)
            {
                for (const auto& memory : memories.memory_container[i].memory_storage)
                {
                    auto* ResourceLocation = dynamic_cast<memory::ResourceLocation*>(memory.get());
                    if (ResourceLocation)
                    {
                        glm::vec3 color{0.f, 0.f, 0.f};
                        if (ResourceLocation->m_tags & ETag::TAG_Food)
                        {
                            color.r = 1.f;
                        }
                        else if (ResourceLocation->m_tags & ETag::TAG_Drink)
                        {
                            color.g = 1.f;
                        }
                        gfx::get_renderer().debug().draw_circle(ResourceLocation->m_location, vision.radius, color);
                    }
                }
            }
        });
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
        auto vis_rad = m_context.registry->try_get<component::Vision>(e)->radius;
        auto* o_res  = dynamic_cast<memory::ResourceLocation*>(old_memory.get());
        auto* n_res  = dynamic_cast<memory::ResourceLocation*>(memory);
        if (o_res && n_res && close_enough(o_res->m_location, n_res->m_location, vis_rad))
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
        auto res_f = dynamic_cast<memory::ResourceLocation*>(container.memory_storage.front().get());
        auto res_b = dynamic_cast<memory::ResourceLocation*>(container.memory_storage.back().get());
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
