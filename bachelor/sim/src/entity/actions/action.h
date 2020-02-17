#pragma once

#include "requirement.h"

#include <functional>
#include <string>
#include <vector>

namespace cs::action
{
struct Action
{
    std::string name{};

    std::vector<std::unique_ptr<IRequirement>> requirements{};

    float time_to_complete{};
    float time_spent= 0.f;

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
        name(p_name),
        requirements(p_requirements),
        time_to_complete(p_time_to_complete),
        abort(p_abort),
        success(p_success),
        failure(p_failure){};

    ~Action() = default;
    Action(const Action& other)
    {
        for (int i = 0; i < other.requirements.size(); i++)
        {
            auto requirement = other.requirements[i].get()->clone();
            requirements.emplace_back(requirement);
        }
    }
};
} // namespace cs::action
