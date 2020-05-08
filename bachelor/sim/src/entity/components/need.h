#pragma once
#include "tags.h"

#include <functional>
#include <string>
#include <variant>

namespace cs::ai
{
/**
 * A need represents the desire of an entity to do something, or more precisely, to fullfill a craving
 *
 * The needs are used by the need and mitigation system to influence an entities decision making, by telling it what it needs to
 * fix and how badly it needs fixing. To this end, needs typically grow more pressing the older they become, with different needs
 * growing exponentially more pressing over their lifespan.
 */
struct Need
{
    using WeightFunction = std::variant<sol::function, std::function<float(const Need&)>>;

    std::string name{};

    /** How important fixing a need right now is. Usually determined via a weight function */
    float weight{};

    /** What the current status of the need is ranging from 1 (fullfilled) to 0 (unfullfilled) */
    float status{};

    /** The threshhold at which the need will be added to vital needs */
    float threshhold{};

    /** How much fullfillment a need looses every second */
    float decay_rate{};

    /** How much a need impacts the health of an agent when reaching 0 */
    float vitality{};

    ETag tags{};

    /**
     * weight_func is a user implementable function that determines the importance of the need.
     *
     * @param local A reference to the Need in question necessary due to Lua instantialization
     *
     * @return the importance of fullfilling the need. default is the deficit in status multiplied by the weight and multiplier
     */
    WeightFunction weight_func = [](const ai::Need& local) {
        return (100 / (local.status + 1)) * local.weight * local.weight_multi;
    };

    /** Is only used with traits */
    float weight_multi{1.f};
    float decay_multi{1.f};
};

inline bool operator<(Need const& lhs, Need const& rhs)
{
    float lhs_f = 0, rhs_f = 0;
    if (lhs.weight_func.index() == 0)
    {
        lhs_f = std::get<sol::function>(lhs.weight_func)(lhs);
    }
    else
    {
        lhs_f = std::get<std::function<float(const Need&)>>(lhs.weight_func)(lhs);
    }

    if (rhs.weight_func.index() == 0)
    {
        rhs_f = std::get<sol::function>(rhs.weight_func)(rhs);
    }
    else
    {
        rhs_f = std::get<std::function<float(const Need&)>>(rhs.weight_func)(rhs);
    }

    return lhs_f < rhs_f;
}

inline bool operator>(Need const& lhs, Need const& rhs)
{
    float lhs_f = 0, rhs_f = 0;
    if (lhs.weight_func.index() == 0)
    {
        lhs_f = std::get<sol::function>(lhs.weight_func)(lhs);
    }
    else
    {
        lhs_f = std::get<std::function<float(const Need&)>>(lhs.weight_func)(lhs);
    }

    if (rhs.weight_func.index() == 0)
    {
        rhs_f = std::get<sol::function>(rhs.weight_func)(rhs);
    }
    else
    {
        rhs_f = std::get<std::function<float(const Need&)>>(rhs.weight_func)(rhs);
    }

    return lhs_f > rhs_f;
}

inline bool operator==(Need const& lhs, Need const& rhs)
{
    return lhs.name == rhs.name;
}

inline bool operator!=(Need const& lhs, Need const& rhs)
{
    return !(lhs == rhs);
}
} // namespace cs::ai
