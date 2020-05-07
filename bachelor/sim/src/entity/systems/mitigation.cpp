#include "mitigation.h"
#include "common_helpers.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/strategy.h"
#include "entt/entity/registry.hpp"

#include <spdlog/spdlog.h>

namespace cs::system
{
void Mitigation::initialize()
{
    m_context.dispatcher->sink<event::SwitchNeedContext>().connect<&Mitigation::switch_need_context>(this);
}

void Mitigation::deinitialize()
{
    m_context.dispatcher->sink<event::SwitchNeedContext>().disconnect<&Mitigation::switch_need_context>(this);
}
void Mitigation::update(float dt)
{
    CS_AUTOTIMER(Mitigation System);

    auto view = m_context.registry->view<component::Need, component::Strategy, component::Tags>();
    view.each([this](entt::entity e, component::Need& needs, component::Strategy& strategies, const component::Tags& tags) {
        if (!needs.vital_needs.empty())
        {
            const auto temp = needs.vital_needs.front();

            /** Put the most pressing needs to the front*/
            std::sort(needs.vital_needs.begin(), needs.vital_needs.end(), std::greater<ai::Need>());

            /** If the most pressing need has changed */
            if (temp != needs.vital_needs.front())
            {
                strategies.staged_strategies.clear();
                m_context.registry->remove_if_exists<component::LocationRequirement>(e);
                m_context.registry->remove_if_exists<component::VisionRequirement>(e);
                m_context.registry->remove_if_exists<component::FindRequirement>(e);
                m_context.registry->remove_if_exists<component::TagRequirement>(e);
            }

            if (strategies.staged_strategies.empty())
            {
                if (!(add_strategies(strategies, needs.vital_needs.front(), tags)))
                {
                    spdlog::get("agent")->warn("Unable to add actions to fix need {}", needs.vital_needs[0].name);
                }
            }
        }
        else if (!needs.leisure_needs.empty())
        {
            if (strategies.staged_strategies.empty())
            {
                if (!(add_strategies(
                        strategies,
                        needs.leisure_needs[m_context.rng->uniform(0, static_cast<int>((needs.leisure_needs.size() - 1)))],
                        tags)))
                {
                    spdlog::get("agent")->warn("Unable to add actions to satisfy leisure need");
                }
            }
        }
        else if (!strategies.staged_strategies.empty())
        {
            strategies.staged_strategies.clear();
            m_context.registry->remove_if_exists<component::LocationRequirement>(e);
            m_context.registry->remove_if_exists<component::VisionRequirement>(e);
            m_context.registry->remove_if_exists<component::FindRequirement>(e);
            m_context.registry->remove_if_exists<component::TagRequirement>(e);
        }
    });
}

ISystem* Mitigation::clone()
{
    return new Mitigation(m_context);
}

bool Mitigation::add_strategies(component::Strategy& strategies, const ai::Need& need, const component::Tags& tags)
{
    ai::Strategy temp{};

    /** Find actions with tags that match any tag of the need */
    for (const auto& strategy : strategies.strategies)
    {
        /** Bitwise operation have very low costs so we check everything we can bitwise before going into the "slightly" more
         costly count_set_bits function */

        /** Check if the entities tags match the requirements of the strategy */
        if ((strategy.prerequisites & tags.tags) == strategy.prerequisites)
        {
            /** Check if ANY of the strategies tags matches the needs tags */
            if ((strategy.tags & need.tags) != 0)
            {
                const auto matching_tags = count_set_bits(strategy.tags & need.tags);
                temp                     = strategy;
                temp.desirability += matching_tags;
                strategies.staged_strategies.push_back(temp);
            }
        }
    }

    if (!strategies.staged_strategies.empty())
    {
        std::sort(strategies.staged_strategies.begin(), strategies.staged_strategies.end());
        return true;
    }
    return false;
}

void Mitigation::switch_need_context(const event::SwitchNeedContext& event)
{
    if (m_context.registry->valid(event.entity))
    {
        auto strategies = m_context.registry->try_get<component::Strategy>(event.entity);
        if (strategies)
        {
            strategies->staged_strategies.clear();
        }
        else
        {
            spdlog::get("agent")->debug("tried to clear strategies from an entity that does not have that component. Entity: {}",
                                        event.entity);
        }
        m_context.registry->remove_if_exists<component::LocationRequirement>(event.entity);
        m_context.registry->remove_if_exists<component::VisionRequirement>(event.entity);
        m_context.registry->remove_if_exists<component::FindRequirement>(event.entity);
        m_context.registry->remove_if_exists<component::TagRequirement>(event.entity);

        if (auto mov = m_context.registry->try_get<component::Movement>(event.entity); mov)
        {
            mov->desired_position.clear();
        }
    }
}

} // namespace cs::system
