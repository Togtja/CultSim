#pragma once
#include <functional>
#include <string>

namespace cs::action
{
struct Requirement
{
    std::string name;

    std::function<bool(void)> Init{};
    std::function<bool(void)> Predicate{};
};
} // namespace cs::action
