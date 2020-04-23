#pragma once
#include "tags.h"

#include <functional>
#include <string>
#include <variant>

#include <sol/sol.hpp>

namespace cs::gob
{
class Goal
{
public:
    std::string m_name{};
    ETag m_tags{};

    float m_weight{};
    float m_age{};

    std::variant<sol::function, std::function<float()>> m_weight_function = [this]() {
        return m_age;
    };
    std::variant<sol::function, std::function<float(const float)>> m_change_over_time = [this](const float dt) {
        return dt;
    };
};
} // namespace cs::gob