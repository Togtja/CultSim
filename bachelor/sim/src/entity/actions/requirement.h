#pragma once

namespace cs::action
{
class IRequirement
{
private:
    virtual void success() = 0;
    virtual void failure() = 0;

public:
    virtual void update(float dt) = 0;
};
} // namespace cs::action
