#pragma once
#include "tags.h"

#include <functional>
#include <string>
#include <variant>

#include <sol/function.hpp>

namespace cs::gob
{
/**
 * A Goal is an abstract concept representing a entities desire to fullfill a certain need, which is used by the Goal system
 *
 * Goals are simple constructs that hold the information necessary for an entity to correctly track and fullfill it's desires.
 * They have user implementable functions which are used by the goal system to find their importance, and which actions are most
 * desirable for fullfilling them
 */
struct Goal
{
    using WeightFunction = std::variant<sol::function, std::function<float()>>;
    using GoalFunction   = std::variant<sol::function, std::function<float(const float)>>;

    std::string name{};

    ETag tags{};

    /** How long the goal has existed for without being fullfilled. Successfully fullfilling the goal resets this to 0 */
    float age{};

    /**
     * weight_function returns how the weight of the goal. This may be user implemented via lua
     *
     * @return The weight of the goal. This is equal to age by default
     */
    WeightFunction weight_function = [this]() {
        return age;
    };

    /**
     * change_over_time checks how much the goals weight will change over a timeframe. This function may be user implemented
     *
     * @param dt The amount of time to spend in seconds
     *
     * @return The change in weight over the period of time send in. By default this is equal to time spent
     */
    GoalFunction change_over_time = [this](const float dt) {
        return dt;
    };

    /**
     * get_discontentment is a user implementable function that shows how unhappy the entity is with the  new_value for the goal
     *
     * @param new_value The Value to check discontentment for
     *
     * @return How dissatisfying the new value is for this goal. Default is the value squared.
     */
    GoalFunction get_discontentment = [this](const float new_value) {
        return pow(new_value, 2.f);
    };
};
} // namespace cs::gob
