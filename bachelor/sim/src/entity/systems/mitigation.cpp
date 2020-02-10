#include "mitigation.h"
#include "entity/components/components.h"
#include "entity/components/need.h"

#include "spdlog/spdlog.h"

namespace cs::system
{
void Mitigation::update(float dt)
{
    auto view = m_registry.view<component::Needs, component::Actions>();
    view.each([this, dt](component::Needs& needs, component::Actions& actions) {
        if (!needs.pressing_needs.empty())
        {
            auto temp = needs.pressing_needs;
            // Put the most pressing needs to the front
            std::sort(needs.pressing_needs.begin(), needs.pressing_needs.end());

            // If the most pressing need has changed
            if (!(temp[0] == needs.pressing_needs[0]))
            {
                actions.actions.clear();
            }

            if (actions.actions.empty())
            {
                if (!(add_actions(actions, needs.pressing_needs[0])))
                {
                    spdlog::warn("Unable to add actions to fix need {}", needs.pressing_needs[0].name);
                }
            }
        }
    });
}
bool Mitigation::add_actions(component::Actions& actions, const ai::Need& need)
{
    std::vector<ai::Action> temp{};

    for (auto tag : need.tag)
    {
        // Find actions with tags that match any tag of the need
        for (auto action : actions.actions)
        {
            if (std::find(action.tags.begin(), action.tags.end(), tag) != action.tags.end())
            {
                temp.push_back(action);
            }
        }
    }

    return false;
}
} // namespace cs::system
