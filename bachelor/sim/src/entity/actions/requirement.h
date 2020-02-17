#pragma once
#include "interface_requirement.h"

#include <functional>
#include <string>

#include "entt/signal/dispatcher.hpp"

namespace cs::action
{
template<typename T>
struct Requirement : public IRequirement
{

    entt::dispatcher& dispatcher{};

    Requirement(std::string p_name, bool p_predicate, std::function<bool(void)> p_init, entt::dispatcher& p_dispatcher, std::function<void(const T&)> p_event_listener) :
        IRequirement(p_name,p_predicate,p_init),
        dispatcher(p_dispatcher),
        event_listener(p_event_listener)
    {
        dispatcher.sink<T>().connect<&Requirement::event_listener>(this);
    }
    void std::function<void(const T&)> event_listener{};
};
} // namespace cs::action
