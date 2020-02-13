#pragma once

#include "requirement.h"

#include <string>
#include <vector>

namespace cs::action
{
class IAction
{
protected:
    std::vector<Requirement> m_requirements{};
    float m_time_to_complete{};
    float m_time_left{};

    virtual void success() = 0;
    virtual void failure() = 0;

public:
    std::string m_name{};

    IAction(std::string name, float time_to_complete, float time_left, std::vector<Requirement> requirements) :
        m_name(name),
        m_time_to_complete(time_to_complete),
        m_time_left(time_left),
        m_requirements(requirements){};

    virtual void update(float dt) = 0;
};
} // namespace cs::action
