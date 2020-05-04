#pragma once
#include "entity/components/action_sequence.h"
#include "entity/components/goal.h"
#include "system.h"

#include <memory>
#include <vector>

namespace cs::system
{
class GOB : public ISystem
{
    using ISystem::ISystem;

    void update(float dt) override;

    float calculate_discontentment(entt::entity e, const gob::ActionSequence& action, const std::vector<gob::Goal>& goals);

    ISystem* clone() override;
};
} // namespace cs::system
