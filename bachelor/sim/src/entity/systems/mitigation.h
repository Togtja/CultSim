#pragma once
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "system.h"

#include <vector>

namespace cs::system
{
class Mitigation : public ISystem
{
private:
public:
    using ISystem::ISystem;

    void update(float dt) override;

    bool add_strategies(component::Strategies& actions, const ai::Need& need, const component::Tags& tags);
};
} // namespace cs::system
