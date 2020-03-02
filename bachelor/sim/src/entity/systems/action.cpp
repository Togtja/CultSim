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
    auto group     = registry.group<component::Strategies>(entt::exclude<component::LocationRequirement,
                                                                     component::VisionRequirement,
                                                                     component::FindRequirement,
                                                                     component::TagRequirement>);

    group.each([this, dt](entt::entity e, component::Strategies& strategies) {
        if (!strategies.staged_strategies.empty())
        {
            for (auto& strategy : strategies.staged_strategies)
            {
                if (!strategy.actions.empty())
                {
                    auto& action = strategy.actions.back();
                    if (action.requirements != 0)
                    {
                        if (action.requirements & TAG_Tag)
                        {
                            m_context.registry->assign<component::TagRequirement>(e, strategy.tags);
                            action.requirements = static_cast<ETag>(action.requirements & ~TAG_Tag);
                        }
                        else if (action.requirements & TAG_Location)
                        {
                            m_context.registry->assign<component::LocationRequirement>(e, glm::vec3{20.f, 20.f, 0.f});
                            action.requirements = static_cast<ETag>(action.requirements & ~TAG_Location);
                        }
                        else if (action.requirements & TAG_Vision)
                        {
                            m_context.registry->assign<component::VisionRequirement>(e, strategy.tags);
                            action.requirements = static_cast<ETag>(action.requirements & ~TAG_Vision);
                        }
                        else if (action.requirements & TAG_Find)
                        {
                            m_context.registry->assign<component::FindRequirement>(e, strategy.tags, glm::vec3{});
                            action.requirements = static_cast<ETag>(action.requirements & ~TAG_Find);
                        }
                    }
                    else
                    {
                        action.time_spent += dt;
                        if (action.time_spent >= action.time_to_complete)
                        {
                            if (m_context.registry->valid(action.target))
                            {
                                if (m_context.rng->trigger(0.9))
                                {
                                    action.success(e, action.target, *m_context.registry);
                                }

                                else
                                {
                                    action.failure(action.target, *m_context.registry);
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
