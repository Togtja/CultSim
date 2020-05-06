#pragma once
#include "tags.h"

#include <functional>
#include <string>
#include <variant>

#include <sol/function.hpp>

namespace cs::gob
{
/** TODO: Write Documentation*/
/** TODO: Rename all names*/
struct Goal
{
    using WeightFunction = std::variant<sol::function, std::function<float()>>;
    using GoalFunction   = std::variant<sol::function, std::function<float(const float)>>;

    std::string m_name{};
    ETag m_tags{};

    float m_age{};

    WeightFunction m_weight_function = [this]() {
        return m_age;
    };

    GoalFunction m_change_over_time = [this](const float dt) {
        return dt;
    };

    GoalFunction m_get_discontentment = [this](const float new_value) {
        return pow(new_value, 2.f);
    };
};
} // namespace cs::gob
