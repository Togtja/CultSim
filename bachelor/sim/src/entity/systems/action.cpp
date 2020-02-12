
#include "action.h"
#include "..\components\action.h"
#include "entity/components/components.h"
namespace cs::system
{
void Action::update(float dt)
{
    auto view = m_registry.view<component::Strategies>();
    view.each([this, dt](component::Strategies& strategies) {
        if (!strategies.staged_strategies.empty())
        {
            for (auto strategy : strategies.staged_strategies)
            {
                for (auto action : strategy.actions)
                {
                    action.run();
                    // Let run while the action is not finished / Aborted
                    strategies.staged_strategies.clear();
                    break;
                }
            }
        }
    });
}
} // namespace cs::system
