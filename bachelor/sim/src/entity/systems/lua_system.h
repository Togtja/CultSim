#pragma once

#include "system.h"

#include <sol/table.hpp>

namespace cs::system
{
class LuaSystem : public ISystem
{
private:
    /** Lua object that holds the implementations */
    sol::table m_data{};

public:
    LuaSystem(SystemContext context, sol::table data);

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
