#include "input_handler.h"

#include <algorithm>

#include <spdlog/spdlog.h>

namespace cs::input
{
namespace detail
{
InputHandler::InputHandler(KeyContext type)
{
    m_context_type = type;
}

void InputHandler::bind_key(const SDL_Scancode event, const std::function<void()> function)
{
    if (!has_event(event))
    {
        m_key_binding.emplace(event, function);
    }
    else
    {
        spdlog::debug("this context (id: {}) already has a binding for this event ({})", m_context_type, event);
    }
}
void InputHandler::unbind_key(const SDL_Scancode event)
{
    // erase returns how many elements it erased
    if (m_key_binding.erase(event) == 0)
    {
        spdlog::debug("this context (id: {}) doesnot have a binding for this event ({})", m_context_type, event);
    }
}

void InputHandler::handle_input(const SDL_Scancode event)
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

bool InputHandler::has_event(const SDL_Scancode event)
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

void ContextHandler::add_context(KeyContext context)
{
    if (!m_active_stack.empty() && context == KeyContext::DefaultContext)
    {
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

void ContextHandler::remove_context(KeyContext context)
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

void ContextHandler::bind_key(KeyContext context, const SDL_Scancode event, const std::function<void()> function)
{
    auto input_it = m_input_map.find(context);
    if (input_it != m_input_map.end())
    {
        m_input_map.at(context).bind_key(event, function);
    }
    else
    {
        detail::InputHandler new_input{context};
        new_input.bind_key(event, function);
        m_input_map.emplace(context, new_input);
    }
}
void ContextHandler::unbind_key(KeyContext context, const SDL_Scancode event)
{
    auto input_it = m_input_map.find(context);
    if (input_it != m_input_map.end())
    {
        auto input = m_input_map.at(context);
        input.unbind_key(event);
    }
    else
    {
        spdlog::debug("could not unbind, because no input created for the context (id: {})", context);
    }
}

void ContextHandler::handle_input(const SDL_Scancode event)
{
    // Iterate over the the active context stack
    for (auto it = m_active_stack.crbegin(); it != m_active_stack.crend(); it++)
    {
        if (has_context(*it))
        {
            m_input_map.at(*it).handle_input(event);
        }
        return;
    }
    spdlog::debug("could not find anything for the {} event", event);
}
bool ContextHandler::has_context(KeyContext context)
{
    return m_input_map.contains(context);
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

ContextHandler& get_input()
{
    static ContextHandler instance{};
    return instance;
}

} // namespace cs::input
