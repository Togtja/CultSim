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

    virtual void success() = 0;
    virtual void failure() = 0;

public:
    std::string m_name{};

    IAction(std::string name, std::vector<Requirement> requirements) : m_name(name), m_requirements(requirements){};

    virtual void update(float dt) = 0;
};
} // namespace cs::action
