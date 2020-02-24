#include "input_handler.h"

#include <algorithm>

#include <spdlog/spdlog.h>

namespace cs::input
{
namespace detail
{
ActionHandler::ActionHandler(const KeyContext type)
{
    m_context_type = type;
}

void ActionHandler::set_blocking(bool blocking)
{
    m_blocking = blocking;
}

void ActionHandler::bind_action(const Action action, const std::function<void()>& function)
{
    m_action_binding[action] = function;
}

void ActionHandler::bind_key(const SDL_Scancode scancode, const Action action)
{
    m_key_binding[scancode] = action;
}

void ActionHandler::bind_btn(const Uint8 button, const Action action)
{
    m_mouse_binding[button] = action;
}

void ActionHandler::unbind_action(const Action action)
{
    auto&& it = m_action_binding.find(action);
    if (it == m_action_binding.end())
    {
        spdlog::debug("this context (id: {}) does not have a binding for this action ({})", m_context_type, action);
        return;
    }
    m_action_binding.erase(it);
}

void ActionHandler::unbind_key(const SDL_Scancode scancode)
{
    auto&& it = m_key_binding.find(scancode);
    if (it == m_key_binding.end())
    {
        spdlog::debug("this context (id: {}) does not have a binding for this key ({})", m_context_type, scancode);
        return;
    }
    m_key_binding.erase(it);
}

void ActionHandler::unbind_btn(const Uint8 button)
{
    auto&& it = m_mouse_binding.find(button);
    if (it == m_mouse_binding.end())
    {
        spdlog::debug("this context (id: {}) does not have a binding for this button ({})", m_context_type, button);
        return;
    }
    m_mouse_binding.erase(it);
}

bool ActionHandler::handle_input(const SDL_Scancode scancode)
{
    if (has_event(scancode))
    {
        // Call the function the scancode maps to
        auto& action = m_key_binding.at(scancode);
        m_action_binding.at(action)();
    }
    else
    {
        spdlog::debug("no keybinding for {} exist from before in context id: {}", scancode, m_context_type);
    }
    return m_blocking;
}
bool ActionHandler::handle_input(const Uint8 button)
{
    if (has_event(button))
    {
        // Call the function the button maps to
        auto& action = m_mouse_binding.at(button);
        m_action_binding.at(action)();
    }
    else
    {
        spdlog::debug("no keybinding for {} exist from before in context id: {}", button, m_context_type);
    }
    return m_blocking;
}

bool ActionHandler::has_event(const SDL_Scancode scancode)
{
    return m_key_binding.contains(scancode);
}
bool ActionHandler::has_event(const Uint8 button)
{
    return m_mouse_binding.contains(button);
}

void ActionHandler::clear()
{
    m_action_binding.clear();
    m_key_binding.clear();
    m_mouse_binding.clear();
}

ActionHandler::~ActionHandler() = default;

} // namespace detail

/******** CONTEXT HANDLER *********/

void ContextHandler::add_context(const KeyContext context, bool blocking)
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
    if (!has_context(context))
    {
        m_input_map.emplace(context, detail::ActionHandler{context});
    }
    m_input_map.at(context).set_blocking(blocking);
    m_active_stack.push_back(context);
}

void ContextHandler::remove_context(const KeyContext context)
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

void ContextHandler::bind_action(KeyContext context, const Action action, const std::function<void()>& function)
{
    get_action_handler(context).bind_action(action, function);
}

void ContextHandler::bind_key(const KeyContext context, const SDL_Scancode scancode, const Action action)
{
    get_action_handler(context).bind_key(scancode, action);
}

void ContextHandler::bind_btn(const KeyContext context, Uint8 button, const Action action)
{
    get_action_handler(context).bind_btn(button, action);
}

void ContextHandler::unbind_action(const KeyContext context, const Action action)
{
    if (has_context(context))
    {
        m_input_map.at(context).unbind_action(action);
    }
}

void ContextHandler::unbind_key(const KeyContext context, const SDL_Scancode scancode)
{
    if (has_context(context))
    {
        m_input_map.at(context).unbind_key(scancode);
    }
}

void ContextHandler::unbind_btn(const KeyContext context, const Uint8 button)
{
    if (has_context(context))
    {
        m_input_map.at(context).unbind_btn(button);
    }
}

void ContextHandler::handle_input(const SDL_Scancode scancode)
{
    // Iterate over the the active context stack
    for (auto it = m_active_stack.crbegin(); it != m_active_stack.crend(); it++)
    {
        if (has_context(*it) && m_input_map.at(*it).has_event(scancode))
        {
            m_input_map.at(*it).handle_input(scancode);
            return;
        }
    }
    spdlog::debug("could not find anything for the {} scancode", scancode);
}

void ContextHandler::handle_input(const Uint8 button)
{
    // Iterate over the the active context stack
    for (auto it = m_active_stack.crbegin(); it != m_active_stack.crend(); it++)
    {
        if (has_context(*it) && m_input_map.at(*it).has_event(button))
        {
            m_input_map.at(*it).handle_input(button);
            return;
        }
    }
    spdlog::debug("could not find anything for the {} button", button);
}

bool ContextHandler::has_context(const KeyContext context)
{
    return m_input_map.contains(context);
}

void ContextHandler::clear_context(const KeyContext context)
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
