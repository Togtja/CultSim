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

    gob::Action_Sequence choose_action(std::vector<gob::Action_Sequence> actions, std::vector<gob::Goal> goals);

    float calculate_discontentment(gob::Action_Sequence action, std::vector<gob::Goal> goals);

    ISystem* clone() override;
};
} // namespace cs::system
