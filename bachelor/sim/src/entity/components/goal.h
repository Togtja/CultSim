#pragma once
#include<string>
#include <variant>

namespace cs::gob
{
class Goal
{
    std::string name{};
    ETag tags{};

    float weight{};
    float age{};

    
    std::variant<sol::function, std::function<float(const Need&)>> weight_func = [](const ai::Need& local) {
        return (100 / (local.status + 1)) * local.weight * local.weight_multi;
    };
};
} // namespace cs::gob