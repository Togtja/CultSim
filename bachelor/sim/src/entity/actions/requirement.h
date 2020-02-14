#pragma once
#include <functional>
#include <string>

namespace cs::action
{
struct Requirement
{
    std::string name;
    std::function<bool(void)> init{};
    std::function<bool(void)> predicate{};
};
} // namespace cs::action
