#pragma once
#include <functional>
#include <string>

namespace cs::action
{
struct Requirement
{
    std::string name;

    std::function<bool> Init{};
    std::function<void> Predicate{};
};
} // namespace cs::action
