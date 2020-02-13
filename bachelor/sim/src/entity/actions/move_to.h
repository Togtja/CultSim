#pragma once
#include "requirement.h"

#include "glm/glm.hpp"

namespace cs::action
{
class MoveTo : public IRequirement
{
private:
    void success() override;
    void failure() override;

    glm::vec3 m_pos{};

public:
    MoveTo(glm::vec3 pos) : m_pos(pos){};
    void update(float dt) override;
};
} // namespace cs::action
