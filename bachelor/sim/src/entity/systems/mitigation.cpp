#include "mitigation.h"
#include "common_helpers.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/strategy.h"

#include "spdlog/spdlog.h"

namespace cs::system
{
void Mitigation::update(float dt)
{
    CS_AUTOTIMER(Mitigation System);

    auto& registry = *m_context.registry;

    auto view = registry.view<component::Needs, component::Strategies, component::Tags>();
    view.each([this, dt](component::Needs& needs, component::Strategies& strategies, component::Tags tags) {
        if (!needs.vital_needs.empty())
        {
            for (auto need : needs.vital_needs) {}
            auto temp = needs.vital_needs;
            // Put the most pressing needs to the front
            std::sort(needs.vital_needs.begin(), needs.vital_needs.end(), std::greater<ai::Need>());

            // If the most pressing need has changed
            if (!(temp[0] == needs.vital_needs[0]))
            {
                strategies.staged_strategies.clear();
            }

            if (strategies.staged_strategies.empty())
            {
                if (!(add_strategies(strategies, needs.vital_needs[0], tags)))
                {
                    spdlog::get("agent")->warn("Unable to add actions to fix need {}", needs.vital_needs[0].name);
                }
            }
        }
        else if (!needs.leisure_needs.empty())
        {
            if (!strategies.staged_strategies.empty())
            {
                for (auto need : needs.leisure_needs)
                {
                    RandomEngine rng{};
                    if (!(add_strategies(strategies, needs.leisure_needs[rng.uniform(0, static_cast<int>((needs.leisure_needs.size() - 1)))], tags)))
                    {
                        spdlog::get("agent")->warn("Unable to add actions to satisfy leisure need");
                    }
                }
            }
        }
        else
        {
            strategies.staged_strategies.clear();
        }
    });
}

bool Mitigation::add_strategies(component::Strategies& strategies, const ai::Need& need, const component::Tags& tags)
{
    ai::Strategy temp{};

    // Find actions with tags that match any tag of the need
    for (auto strategy : strategies.strategies)
    {
        // Bitwise operation have very low costs so we check everything we can bitwise before going into the "slightly" more
        // costly count_set_bits function

        // Check if the entities tags match the requirements of the strategy
        if ((strategy.requirements & tags.tags) == strategy.requirements)
        {
            // Check if ANY of the strategies tags matches the needs tags
            if ((strategy.tags & need.tags) != 0)
            {
                auto matching_tags = count_set_bits(strategy.tags & need.tags);
                temp               = strategy;
                temp.desirability += matching_tags;
                strategies.staged_strategies.push_back(temp);
            }
        }
    }

    // TODO: Add checks for agents personality and personal history to further increase / decrease strategy desirability

    if (!strategies.staged_strategies.empty())
    {
        std::sort(strategies.staged_strategies.begin(), strategies.staged_strategies.end(), std::greater<ai::Strategy>());
        return true;
    }
    return false;
}
} // namespace cs::system
