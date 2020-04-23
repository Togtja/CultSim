#pragma once
#include <string>
#include <variant>

namespace cs::gob
{
class Goal
{
public:
    std::string name{};
    ETag tags{};

    float weight{};
    float age{};

    std::variant<sol::function, std::function<float>> weight_function = [this]() {
        return age;
    };
    std::variant<sol::function, std::function<float(const float)>> change_over_time = [this](const float dt) {
        return dt;
    };
};
} // namespace cs::gob
