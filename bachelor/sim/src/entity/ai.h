#pragma once
#include "entity/system.h"
#include <glm/vec2.hpp>

namespace cs::system
{
class AI : public ISystem
{
private:
public:
    using ISystem::ISystem;
    bool is_visible();
    bool is_colliding();
    void update(float dt) override;
};

} // namespace cs::system
