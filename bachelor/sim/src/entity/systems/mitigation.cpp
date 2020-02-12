#include "mitigation.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/strategy.h"

#include "spdlog/spdlog.h"

namespace cs::system
{
void Mitigation::update(float dt)
{
    auto view = m_registry.view<component::Needs, component::Strategies>();
    view.each([this, dt](component::Needs& needs, component::Strategies& strategies) {
        if (!needs.pressing_needs.empty())
        {
            auto temp = needs.pressing_needs;
            // Put the most pressing needs to the front
            std::sort(needs.pressing_needs.begin(), needs.pressing_needs.end());

            // If the most pressing need has changed
            if (!(temp[0] == needs.pressing_needs[0]))
            {
                strategies.staged_strategies.clear();
            }

            if (strategies.staged_strategies.empty())
            {
                if (!(add_actions(strategies, needs.pressing_needs[0])))
                {
                    spdlog::warn("Unable to add actions to fix need {}", needs.pressing_needs[0].name);
                }
            }
        }
    });
}
bool Mitigation::add_actions(component::Strategies& strategies, const ai::Need& need)
{
    std::vector<ai::Strategy> temp{};

    for (auto tag : need.tag)
    {
        // Find actions with tags that match any tag of the need
        for (auto strategy : strategies.strategies)
        {
            int matching_tags{};
            if ((matching_tags = std::count(strategy.tags.begin(), strategy.tags.end(), tag)) != 0)
            {
                // increase desirability by number of matched tags
                strategy.desirability += matching_tags;

                // If the Action is not allready in our list of temporary actions, add it
                if (std::find(temp.begin(), temp.end(), strategy) != temp.end())
                {
                    temp.push_back(strategy);
                }
            }
        }
    }

    return false;
}
} // namespace cs::system
