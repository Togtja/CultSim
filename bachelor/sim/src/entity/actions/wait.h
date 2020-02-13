#pragma once
#include "action.h"

namespace cs::action
{
class Wait : public IAction
{
public:
    void success() override;
    void failure() override;

private:
    using IAction::IAction;

    void update(float dt) override;
};
} // namespace cs::action
