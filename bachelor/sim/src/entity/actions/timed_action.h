#pragma once

#include "action.h"

namespace cs::action
{
class TimedAction : public IAction
{
private:
    float m_time_to_complete{};
    float m_time_left{};

    void success() override;
    void failure() override;

public:
    TimedAction(std::string name, std::vector<Requirement> requirements, float time_to_complete, float time_left) :
        IAction(name, requirements),
        m_time_to_complete(time_to_complete),
        m_time_left(time_left){};

    void update(float dt) override;
};
} // namespace cs::action
