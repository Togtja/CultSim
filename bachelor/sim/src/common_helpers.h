#pragma once

#include <cstdint>

#include <sol/state_view.hpp>

namespace cs
{
class SceneManager;
class PreferenceManager;

/**
 * The application context struct give other sections of the program access to application wide data
 */
struct ApplicationContext
{
    sol::state_view lua_state;
    SceneManager* scene_manager{nullptr};
    PreferenceManager* preferences{nullptr};
};

int count_set_bits(uint64_t x)
{
    int sum{};
    while (x != 0)
    {
        sum += x & 1;
        x >>= 1;
    }
    return sum;
}
} // namespace cs
