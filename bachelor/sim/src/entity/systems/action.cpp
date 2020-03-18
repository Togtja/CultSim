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
    auto group     = registry.group<component::Strategy>(entt::exclude<component::LocationRequirement,
                                                                   component::VisionRequirement,
                                                                   component::FindRequirement,
                                                                   component::TagRequirement>);

    group.each([this, dt](entt::entity e, component::Strategy& strategies) {
        if (!strategies.staged_strategies.empty())
        {
            auto& strategy = strategies.staged_strategies.back();

            if ((!strategy.actions.empty()) && strategy.working_on_action <= strategy.actions.size())
            {
                // for the first action initialize the requirements when we first start to work
                if (strategy.working_on_action == 0)
                {
                    strategy.working_on_action++;
                    strategy.requirements = strategy.actions.back().requirements;
                }

                auto& action = strategy.actions[strategy.actions.size() - strategy.working_on_action];

                if (strategy.requirements)
                {
                    if (strategy.requirements & TAG_Tag)
                    {
                        m_context.registry->assign<component::TagRequirement>(e, strategy.tags);
                        strategy.requirements = static_cast<ETag>(action.requirements & ~TAG_Tag);
                    }
                    if (strategy.requirements & TAG_Location)
                    {
                        m_context.registry->assign<component::LocationRequirement>(e, glm::vec3{20.f, 20.f, 0.f}, 0.f, 30.f, 0.f);
                        strategy.requirements = static_cast<ETag>(action.requirements & ~TAG_Location);
                    }
                    if (strategy.requirements & TAG_Vision)
                    {
                        m_context.registry->assign<component::VisionRequirement>(e, strategy.tags);
                        strategy.requirements = static_cast<ETag>(action.requirements & ~TAG_Vision);
                    }
                    if (strategy.requirements & TAG_Find)
                    {
                        m_context.registry->assign<component::FindRequirement>(e, strategy.tags, glm::vec3{}, 30.f,0.f);
                        strategy.requirements = static_cast<ETag>(action.requirements & ~TAG_Find);
                    }
                }
                else
                {
                    action.time_spent += dt;
                    if (action.time_spent >= action.time_to_complete)
                    {
                        if (m_context.rng->trigger(action.success_chance))
                        {
                            action.success(e, action.target);
                            action.time_spent     = 0;
                            action                = strategy.actions[(strategy.actions.size() - strategy.working_on_action)];
                            strategy.requirements = action.requirements;
                            if (strategy.working_on_action < strategy.actions.size())
                            {
                                strategy.working_on_action++;
                            }
                        }

                        else
                        {
                            action.failure(e, action.target);
                            action.time_spent     = 0;
                            strategy.requirements = action.requirements;
                        }
                    }
                }
            }
            else
            {
                strategies.staged_strategies.pop_back();
            }
        }
    });
}
void Action::abort_strategy(const event::RequirementFailure& event)
{
    auto strategies = m_context.registry->try_get<component::Strategy>(event.entity);
    if (event.error != "")
    {
        spdlog::get("agent")->error("We got an error from Requirement: {} ; {}", tag_to_string(event.requirement), event.error);
    }

    if (strategies)
    {
        auto& strategy = strategies->staged_strategies.back();
        auto& action   = strategy.actions[strategy.actions.size() - strategy.working_on_action];

        action.abort();

        strategy.desirability--;

        std::sort(strategies->staged_strategies.begin(), strategies->staged_strategies.end());
    }
}
} // namespace cs::system
