#pragma once
#include "tags.h"

#include <functional>
#include <string>
#include <variant>
#include <vector>

namespace cs::ai
{
struct Need
{
    std::string name{};

    float weight{};

    float status{};

    float decay_rate{};

    float vitality{};

    ETag tags{};

    std::variant<sol::function, std::function<float(const Need&)>> weight_func = [](const ai::Need& local) {
        return (100 / (local.status + 1)) * local.weight * local.weight_multi;
    };

    // Is only used with personalities
    float weight_multi = 1.f;
    float decay_multi  = 1.f;
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
    return lhs.name != rhs.name;
}
} // namespace cs::ai
