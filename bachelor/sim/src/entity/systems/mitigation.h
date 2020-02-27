#pragma once
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "system.h"

#include <vector>

namespace cs::system
{
class Mitigation : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;

    /**
     *
     * Go through a list of possible strategies and filters out those that can help in mitigating our pressing needs.
     * Also adds a desirability, and sorts our available strategies accordingly
     *
     * @param strategies The component containing all our available strategies and staged strategies
     * @param need The need we are trying to mitigate
     * @param tags The tags of our Agent. These are used to determine whether we fullfill the requirements for some of the
     * strategies
     *
     */
    bool add_strategies(component::Strategies& strategies, const ai::Need& need, const component::Tags& tags);
};
} // namespace cs::system
