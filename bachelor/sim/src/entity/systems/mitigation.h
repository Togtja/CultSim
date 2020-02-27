#pragma once
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "system.h"

#include <vector>

namespace cs::system
{
/**
* Goes through the list of pressing needs and creates a mitigation strategy for the most pressing one
*
* The Mitigation system goes through the list of pressing needs created by the pressing needs system.
* It then picks out the most pressing of these and goes through a list of available strategies, putting those that match one or more tags into a list of mitigation strategies
* The desirability of those Strategies is determined by how many of their tags match those of the need
* Once a list of strategies has been created it is sorted so that the most desirable strategy is in the front
*/
class Mitigation : public ISystem
{
private:
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
