#include "input_handler.h"

#include <algorithm>

#include <spdlog/spdlog.h>

namespace cs::input
{
namespace detail
{
InputHandler::InputHandler(const KeyContext& type)
{
    m_context_type = type;
}

void InputHandler::bind_key(const SDL_Scancode& event, const std::function<void()>& function, bool overwrite)
{
    if (overwrite)
    {
        m_key_binding[event] = function;
        return;
    }
    if (!has_event(event))
    {
        m_key_binding.emplace(event, function);
    }
    else
    {
        spdlog::debug("this context (id: {}) already has a binding for this event ({})", m_context_type, event);
    }
}
void InputHandler::unbind_key(const SDL_Scancode& event)
{
    auto&& it = m_key_binding.find(event);
    if (it == m_key_binding.end())
    {
        spdlog::debug("this context (id: {}) does not have a binding for this event ({})", m_context_type, event);
        return;
    }
    m_key_binding.erase(it);
}

void InputHandler::handle_input(const SDL_Scancode& event)
{
    if (has_event(event))
    {
        // Call the function the event maps to
        m_key_binding.at(event)();
    }
    else
    {
        spdlog::debug("no keybinding for {} exist from before in context id: {}", event, m_context_type);
    }
}

bool InputHandler::has_event(const SDL_Scancode& event)
{
    return m_key_binding.contains(event);
}

void InputHandler::clear()
{
    m_key_binding.clear();
}

InputHandler::~InputHandler() = default;

} // namespace detail

/******** CONTEXT HANDLER *********/

void ContextHandler::add_context(const KeyContext& context)
{
    if (!m_active_stack.empty() && context == KeyContext::DefaultContext)
    {
        spdlog::warn("can not add default context on top of something else");
        return;
    }

    auto context_it = std::find(std::begin(m_active_stack), std::end(m_active_stack), context);
    /** If it is not at the end then it is already there so remove it and then add it */
    if (context_it != std::end(m_active_stack))
    {
        remove_context(context);
    }
    m_active_stack.push_back(context);
}

void ContextHandler::remove_context(const KeyContext& context)
{
    if (context == KeyContext::DefaultContext)
    {
        spdlog::warn("trying to remove the default context");
        return;
    }
    auto context_it = std::find(std::begin(m_active_stack), std::end(m_active_stack), context);
    /** If it is not the end then we have it */
    if (context_it != std::end(m_active_stack))
    {
        m_active_stack.erase(context_it);
    }
    else
    {
        spdlog::debug("trying to remove context id {} and it is not here from before", context);
    }
}

void ContextHandler::remove_context()
{
    if (m_active_stack.size() <= 1)
    {
        spdlog::warn("trying to remove the default context");
        return;
    }
    m_active_stack.pop_back();
}

void ContextHandler::fast_bind_key(const KeyContext context,
                                   const SDL_Scancode scancode,
                                   const Action action,
                                   const std::function<void()>& function)
{
    bind_key(context, scancode, action);
    bind_action(context, action, function);
}

void ContextHandler::fast_bind_btn(const KeyContext context,
                                   const Uint8 button,
                                   const Action action,
                                   const std::function<void()>& function)
{
    bind_btn(context, button, action);
    bind_action(context, action, function);
}
    {
        m_input_map.at(context).bind_key(event, function, overwrite);
    }
    else
    {
        detail::InputHandler new_input{context};
        new_input.bind_key(event, function);
        m_input_map.emplace(context, new_input);
    }
}
void ContextHandler::unbind_key(const KeyContext& context, const SDL_Scancode& event)
{
    if (has_context(context))
    {
        auto& input = m_input_map.at(context);
        input.unbind_key(event);
    }
    else
    {
        spdlog::debug("could not unbind, because the context (id: {}) does not exist", context);
    }
}

void ContextHandler::handle_input(const SDL_Scancode& event)
{
    // Iterate over the the active context stack
    for (auto it = m_active_stack.crbegin(); it != m_active_stack.crend(); it++)
    {
        if (has_context(*it) && m_input_map.at(*it).has_event(event))
        {
            m_input_map.at(*it).handle_input(event);
            return;
        }
    }
    spdlog::debug("could not find anything for the {} event", event);
}

bool ContextHandler::has_context(const KeyContext& context)
{
    return m_input_map.contains(context);
}

void ContextHandler::clear_context(const KeyContext& context)
{
    if (has_context(context))
    {
        m_input_map.at(context).clear();
    }
    else
    {
        spdlog::debug("the context {} has no bindings", context);
    }
}
void ContextHandler::back_to_default()
{
    m_active_stack.erase(m_active_stack.begin() + 1, m_active_stack.end());
}

void ContextHandler::clear()
{
    m_input_map.clear();
    m_active_stack.clear();
    // There always need to be a DefaultContext at the bottom of the stack
    add_context(KeyContext::DefaultContext);
}

ContextHandler::ContextHandler()
{
    add_context(KeyContext::DefaultContext);
}

detail::ActionHandler& ContextHandler::get_action_handler(KeyContext context)
{
    const auto& input_it = m_input_map.find(context);
    if (input_it == m_input_map.end())
    {
        m_input_map.emplace(context, detail::ActionHandler{context});
    }
    return m_input_map.at(context);
}

ContextHandler& get_input()
{
    static ContextHandler instance{};
    return instance;
}

} // namespace cs::input
