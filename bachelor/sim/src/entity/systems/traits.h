#pragma once

#include "system.h"

namespace cs::system
{
/**
 * The trait system handles assigning traits
 */
class Traits : public ISystem
{
    static inline constexpr int s_chunk_size{128};

public:
    using ISystem::ISystem;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
