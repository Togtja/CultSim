#include "action.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include <random>

#include "spdlog/spdlog.h"

namespace cs::system
{
void Action::update(float dt)
{
    CS_AUTOTIMER(Action System);

    auto& registry = *m_context.registry;
    auto group     = registry.group<component::Strategies>(
        entt::exclude<component::LocationRequirement, component::VisionRequirement, component::FindRequirement>);
    group.each([this, &registry, dt](entt::entity e, component::Strategies& strategies) {
        if (!strategies.staged_strategies.empty())
        {
            for (auto& strategy : strategies.staged_strategies)
            {
                if (!strategy.actions.empty())
                {
                    auto& action = strategy.actions.back();
                    if (action.requirements != 0)
                    {
                        spdlog::warn("Pushing back requirement {}", action.requirements);
                        switch (action.requirements)
                        {
                            case TAG_Location:
                            {
                                spdlog::warn("Pushing to entt: {}", static_cast<uint32_t>(e));
                                registry.assign<component::LocationRequirement>(e, glm::vec3{20.f, 20.f, 0.f});
                                action.requirements = static_cast<ETag>(action.requirements & ~TAG_Location);
                                break;
                            }
                            case TAG_Vision:
                            {
                                registry.assign<component::VisionRequirement>(e, strategy.tags);
                                action.requirements = static_cast<ETag>(action.requirements & ~TAG_Vision);
                                break;
                            }
                            case TAG_Find:
                            {
                                registry.assign<component::FindRequirement>(e, strategy.tags, glm::vec3{});
                                action.requirements = static_cast<ETag>(action.requirements & ~TAG_Find);
                            }
                        }
                        break;
                    }
                    else
                    {
                        action.time_spent += dt;
                        if (action.time_spent >= action.time_to_complete)
                        {
                            if (registry.valid(action.target))
                            {
                                if (m_context.rng->trigger(0.9))
                                {
                                    action.success(e, action.target, registry);
                                }

                                else
                                {
                                    action.failure(action.target, registry);
                                }
                            }
                            strategy.actions.pop_back();
                            break;
                        }
                    }
                }
                else
                {
                    strategies.staged_strategies.erase(strategies.staged_strategies.begin());
                }
            }
        }
    });
}
} // namespace cs::system
