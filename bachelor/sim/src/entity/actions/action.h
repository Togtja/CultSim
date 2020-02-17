#pragma once

#include "requirement.h"

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace cs::action
{
struct Action
{
    std::string name{};

    std::vector<std::unique_ptr<IRequirement>> requirements{};

    float time_to_complete{};
    float time_spent = 0.f;

    std::function<void(void)> abort{};
    std::function<void(void)> success{};
    std::function<void(void)> failure{};

    Action() = default;
    Action(std::string p_name,
           std::vector<std::unique_ptr<IRequirement>> p_requirements,
           float p_time_to_complete,
           std::function<void(void)> p_abort,
           std::function<void(void)> p_success,
           std::function<void(void)> p_failure) :
        name(std::move(p_name)),
        requirements(std::move(p_requirements)),
        time_to_complete(p_time_to_complete),
        abort(std::move(p_abort)),
        success(std::move(p_success)),
        failure(std::move(p_failure)){};

    ~Action() = default;
    Action(const Action& other) : name(other.name), time_to_complete(other.time_to_complete), time_spent(other.time_spent)
    {
        for (const auto& i : other.requirements)
        {
            auto requirement = i->clone();
            requirements.emplace_back(requirement);
        }
    }

    Action& operator=(const Action& other)
    {
        if (this == &other)
        {
            return *this;
        }

        name             = other.name;
        time_to_complete = other.time_to_complete;
        time_spent       = other.time_spent;
        abort            = other.abort;
        success          = other.success;
        failure          = other.failure;

        requirements.clear();
        for (const auto& i : other.requirements)
        {
            auto requirement = i->clone();
            requirements.emplace_back(requirement);
        }

        return *this;
    }
};
} // namespace cs::action
