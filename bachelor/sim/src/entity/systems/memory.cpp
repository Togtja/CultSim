#include "memory.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/memories/resource_memory.h"

#include <glm/glm.hpp>

namespace cs::system
{
void Memory::update(float dt)
{
    CS_AUTOTIMER(Memory System);

    m_timer += dt;
    if (m_timer >= 1)
    {
        m_timer        = 0.f;
        auto& registry = m_context.registry;
        auto view      = registry->view<component::Memory>();

        view.each([dt, registry](entt::entity e, component::Memory& memory) {
            for (auto& memory_storage : memory.memory_storage)
            {
                if (memory_storage.memory_container.size() != 0)
                {
                    if (dynamic_cast<ResourceMemory*>(memory_storage.memory_container.front().get()))
                    {
                        auto pos = registry->get<component::Position>(e).position;
                        std::sort(memory_storage.memory_container.begin(),
                                  memory_storage.memory_container.end(),
                                  [pos](const std::unique_ptr<IMemory>& lhs, const std::unique_ptr<IMemory>& rhs) {
                                      auto res_lhs = dynamic_cast<ResourceMemory*>(lhs.get());
                                      auto res_rhs = dynamic_cast<ResourceMemory*>(rhs.get());

                                      float cost_lhs{};
                                      float cost_rhs{};

                                      cost_lhs = glm::distance(res_lhs->m_location, pos);
                                      cost_rhs = glm::distance(res_rhs->m_location, pos);

                                      cost_lhs -= res_lhs->m_number_of_matching_entities * 10.f;
                                      cost_rhs -= res_rhs->m_number_of_matching_entities * 10.f;

                                      return cost_lhs < cost_rhs;
                                  });

                        int i = 0;
                        for (auto& memory : memory_storage.memory_container)
                        {
                            if (auto res = dynamic_cast<ResourceMemory*>(memory.get());
                                res && res->m_number_of_matching_entities == 0)
                            {
                                memory_storage.memory_container.erase(memory_storage.memory_container.begin() + i);
                            }
                            i++;
                        }

                        // TODO: Remove magic number and put limit of memories remembered into Lua
                        while (memory_storage.memory_container.size() >= 10)
                        {
                            memory_storage.memory_container.erase(memory_storage.memory_container.end() - 1);
                        }
                    }
                }
            }
        });
    }
}
} // namespace cs::system