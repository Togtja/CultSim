#pragma once
#include <functional>
#include <string>

namespace cs::action
{
struct IRequirement
{
    std::string name{};
    bool predicate = false;
    std::function<bool(void)> init{};


    IRequirement(std::string p_name, bool p_predicate, std::function<bool(void)> p_init) :
        name(p_name),
        predicate(p_predicate),
        init(p_init){};
};
} // namespace cs::action
