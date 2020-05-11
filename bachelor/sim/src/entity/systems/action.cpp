#include "action.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "random_engine.h"

#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace cs::system
{
void Action::initialize()
{
    m_context.dispatcher->sink<event::RequirementFailure>().connect<&Action::abort_strategy>(*this);
    m_context.dispatcher->sink<event::EntityDeleted>().connect<&Action::delete_target>(*this);
    m_context.dispatcher->sink<event::EntityPickedUp>().connect<&Action::picked_up_entity>(*this);
}

void Action::deinitialize()
{
    m_context.dispatcher->sink<event::RequirementFailure>().disconnect<&Action::abort_strategy>(*this);
    m_context.dispatcher->sink<event::EntityDeleted>().disconnect<&Action::delete_target>(*this);
    m_context.dispatcher->sink<event::EntityPickedUp>().disconnect<&Action::picked_up_entity>(*this);
}

void Action::update(float dt)
{
    CS_AUTOTIMER(Action System);

    auto& registry = *m_context.registry;
    auto group     = registry.group<component::Strategy>(entt::exclude<component::LocationRequirement,
                                                                   component::VisionRequirement,
                                                                   component::FindRequirement,
                                                                   component::TagRequirement,
                                                                   component::InventoryRequirement>);

    group.each([this, dt](const entt::entity e, component::Strategy& strategies) {
        if (strategies.staged_strategies.empty())
        {
            return;
        }

        auto& strategy = strategies.staged_strategies.back();

        if (strategy.actions.empty() || strategy.working_on_action > strategy.actions.size())
        {
            strategies.staged_strategies.pop_back();
            return;
        }

        /** for the first action initialize the requirements when we first start to work */
        if (strategy.working_on_action == 0)
        {
            strategy.working_on_action++;
            strategy.requirements = strategy.actions.back().requirements;
        }
        auto* action = &strategy.actions[strategy.actions.size() - strategy.working_on_action];

        if (strategy.requirements)
        {
            if (strategy.requirements & TAG_Tag)
            {
                m_context.registry->assign<component::TagRequirement>(e, strategy.target_tags);
                strategy.requirements = static_cast<ETag>(strategy.requirements & ~TAG_Tag);
            }
            if (strategy.requirements & TAG_Location)
            {
                m_context.registry->assign<component::LocationRequirement>(e, glm::vec3{20.f, 20.f, 0.f}, 0.f, 30.f, 0.f);
                strategy.requirements = static_cast<ETag>(strategy.requirements & ~TAG_Location);
            }
            if (strategy.requirements & TAG_Find)
            {
                m_context.registry->assign<component::FindRequirement>(e, strategy.target_tags, glm::vec3{}, 30.f, 0.f);
                strategy.requirements = static_cast<ETag>(strategy.requirements & ~TAG_Find);
            }
            if (strategy.requirements & TAG_Vision)
            {
                m_context.registry->assign<component::VisionRequirement>(e, strategy.target_tags);
                strategy.requirements = static_cast<ETag>(strategy.requirements & ~TAG_Vision);
            }
            if (strategy.requirements & TAG_Inventory)
            {
                m_context.registry->assign<component::InventoryRequirement>(e, 1, strategy.target_tags);
                strategy.requirements = static_cast<ETag>(strategy.requirements & ~TAG_Inventory);
            }
            if (strategy.requirements & ~TAG_None)
            {
                spdlog::warn("Unhandled requirement {}", tag_to_string(strategy.requirements));
            }
            strategy.requirements = TAG_None;
        }
        else
        {
            action->time_spent += dt;

            if (action->time_spent >= action->time_to_complete)
            {
                if ((!m_context.registry->valid(action->target)) || m_context.registry->has<component::Delete>(action->target))
                {
                    action->time_spent    = 0;
                    action->target        = entt::null;
                    strategy.requirements = action->requirements;
                    return;
                }
                if (m_context.rng->trigger(action->success_chance))
                {
                    action->success(e, action->target);
                    action->time_spent = 0;
                    action->target     = entt::null;

                    if (strategy.working_on_action < strategy.actions.size())
                    {
                        strategy.working_on_action++;
                    }
                    else
                    {
                        strategies.staged_strategies.clear();
                        return;
                    }
                    action                = &strategy.actions[strategy.actions.size() - strategy.working_on_action];
                    strategy.requirements = action->requirements;
                }

                else
                {
                    action->failure(e, action->target);
                    action->time_spent    = 0;
                    action->target        = entt::null;
                    strategy.requirements = action->requirements;
                }
            }
        }
    });
}

ISystem* Action::clone()
{
    return new Action(m_context);
}

void Action::abort_strategy(const event::RequirementFailure& event)
{
    if (!m_context.registry->valid(event.entity))
    {
        return;
    }

    if (event.error != "")
    {
        spdlog::get("agent")->error("we got an error from Requirement: {} ; {}", tag_to_string(event.requirement), event.error);
    }

    auto strategies = m_context.registry->try_get<component::Strategy>(event.entity);
    if (strategies)
    {
        auto& strategy = strategies->staged_strategies.back();
        auto& action   = strategy.actions[strategy.actions.size() - strategy.working_on_action];
        if (action.abort.valid() && m_context.registry->valid(action.target))
        {
            action.abort(event.entity, action.target);
        }

        strategy.desirability -= 1.f;

        std::sort(strategies->staged_strategies.begin(), strategies->staged_strategies.end());
    }
}

void Action::delete_target(const event::EntityDeleted& event)
{
    auto view = m_context.registry->view<component::Strategy>();
    view.each([this, event](entt::entity e, component::Strategy& strat) {
        for (auto& strategy : strat.staged_strategies)
        {
            for (auto& action : strategy.actions)
            {
                if (action.target == event.entity)
                {
                    if (action.abort.valid() && m_context.registry->valid(action.target))
                    {
                        action.abort(e, action.target);
                    }
                    action.time_spent     = 0;
                    action.target         = entt::null;
                    strategy.requirements = action.requirements;
                }
            }
        }
    });
}

void Action::picked_up_entity(const event::EntityPickedUp& event)
{
    auto view = m_context.registry->view<component::Strategy>();
    view.each([this, event](entt::entity e, component::Strategy& strat) {
        if (strat.staged_strategies.empty())
        {
            return;
        }

        auto& strategy = strat.staged_strategies.back();
        if (strategy.tags & event.tags && strategy.working_on_action != 0)
        {
            auto& action = strategy.actions[strategy.actions.size() - strategy.working_on_action];
            if (action.target == event.target)
            {
                action.time_spent     = 0;
                action.target         = entt::null;
                strategy.requirements = action.requirements;
            }
        }
    });
}
} // namespace cs::system
