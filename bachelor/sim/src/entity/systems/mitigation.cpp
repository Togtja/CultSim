#include "mitigation.h"
#include "common_helpers.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/strategy.h"

#include "spdlog/spdlog.h"

namespace cs::system
{
void Mitigation::update(float dt)
{
    auto view = m_registry.view<component::Needs, component::Strategies, component::Tags>();
    view.each([this, dt](component::Needs& needs, component::Strategies& strategies, component::Tags tags) {
        if (!needs.pressing_needs.empty())
        {
            auto temp = needs.pressing_needs;
            // Put the most pressing needs to the front
            std::sort(needs.pressing_needs.begin(), needs.pressing_needs.end(), std::greater<ai::Need>());

            // If the most pressing need has changed
            if (!(temp[0] == needs.pressing_needs[0]))
            {
                strategies.staged_strategies.clear();
            }

            if (strategies.staged_strategies.empty())
            {
                if ((add_strategies(strategies, needs.pressing_needs[0], tags)))
                {
                    // TODO: Actually use the staged strategies
                }
                else
                {
                    spdlog::warn("Unable to add actions to fix need {}", needs.pressing_needs[0].name);
                }
            }
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

    if (strategies.staged_strategies.size() != 0)
    {
        std::sort(strategies.staged_strategies.begin(), strategies.staged_strategies.end(), std::greater<ai::Strategy>());
        return true;
    }
    return false;
}
} // namespace cs::system
