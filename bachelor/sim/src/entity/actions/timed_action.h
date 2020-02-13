#pragma once

#include "action.h"

namespace cs::action
{
class ITimedAction : public IAction
{
private:
    float m_time_to_complete{};
    float m_time_left{};

    virtual void success() override = 0;
    virtual void failure() override = 0;

public:
    ITimedAction(std::string name, std::vector<Requirement> requirements, float time_to_complete, float time_left) :
        IAction(name, requirements),
        m_time_to_complete(time_to_complete),
        m_time_left(time_left){};

    virtual void update(float dt) override = 0;
};
} // namespace cs::action
