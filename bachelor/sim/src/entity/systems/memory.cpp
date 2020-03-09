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
    if (m_timer >= 1)
    {
        m_timer        = 0.f;
        auto& registry = m_context.registry;
        auto view      = registry->view<component::Memory>();

        view.each([dt, registry](entt::entity e, component::Memory& memory) {
            for (auto& memory_container : memory.memory_container)
            {
                if (memory_container.memory_storage.size() != 0)
                {
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

                        int i = 0;
                        for (auto& memory : memory_container.memory_storage)
                        {
                            memory->update(1.f);

                            if (auto res = dynamic_cast<memory::ResourceLocation*>(memory.get());
                                res && res->m_number_of_entities == 0)
                            {
                                memory_container.memory_storage.erase(memory_container.memory_storage.begin() + i);
                            }
                            i++;
                        }

                        // TODO: Remove magic number and put limit of memories remembered into Lua
                        while (memory_container.memory_storage.size() >= 10)
                        {
                            memory_container.memory_storage.erase(memory_container.memory_storage.end() - 1);
                        }
                    }
                }
            }
        });
    }
}
} // namespace cs::system
