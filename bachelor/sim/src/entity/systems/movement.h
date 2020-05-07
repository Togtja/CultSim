#pragma once
#include "system.h"

#include <entt/signal/dispatcher.hpp>

namespace cs::system
{
/** TODO: Documentation */
class Movement : public ISystem
{
private:
    float avoid_rotation{35.f};
    float avoid_cd{0.14f};
    float avoid_dist{10.f};
    float avoid_start{15.f};

public:
    using ISystem::ISystem;

    void update(float dt) override;

    void update_imgui() override;

    ISystem* clone() override;
};
} // namespace cs::system
