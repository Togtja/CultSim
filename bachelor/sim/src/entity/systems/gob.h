#pragma once
#include "entity/components/action_sequence.h"
#include "entity/components/goal.h"
#include "system.h"

#include <vector>

namespace cs::system
{
/** TODO: Documentation */
class GOB : public ISystem
{
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;

private:
    float calculate_discontentment(entt::entity e, const gob::ActionSequence& action, const std::vector<gob::Goal>& goals);
};
} // namespace cs::system
