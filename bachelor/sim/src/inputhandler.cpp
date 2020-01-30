#include "inputhandler.h"

#include <algorithm>

#include "spdlog.h"

namespace cs
{
namespace input
{
InputHandler::InputHandler(KeyContext type)
{
    m_context_type = type;
}

void InputHandler::bind_key(const SDL_Scancode event, const std::function<void()> function)
{
    auto key_it = m_key_binding.find(event);
    if (key_it == m_key_binding.end())
    {
        m_key_binding.emplace(event, function);
    }
    else
    {
        spdlog::debug("this context (id: {}) already has a binding for this event ({})", m_context_type, event);
    }
}

void InputHandler::handle_input(const SDL_Scancode event)
{
    auto key_it = m_key_binding.find(event);
    if (key_it != m_key_binding.end())
    {
        // Call the function the event maps to
        m_key_binding.at(event)();
    }
    else
    {
        spdlog::debug("keybinding does not exist from before");
    }
}

InputHandler::~InputHandler()
{
}

/******** CONTEXT HANDLER *********/
ContextHandler::ContextHandler()
{
}

void ContextHandler::add_context(KeyContext context)
{
    auto context_it = std::find(std::begin(m_active_stack), std::end(m_active_stack), context);
    /** If it is the end then it is not already there */
    if (context_it == std::end(m_active_stack))
    {
        m_active_stack.push_back(context);
    }
    else
    {
        spdlog::debug("trying to add context id {} and it is here from before", context);
    }
}

void ContextHandler::remove_context(KeyContext context)
{
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

void ContextHandler::bind_key(KeyContext context, const SDL_Scancode event, const std::function<void()> function)
{
    m_input_map[context].bind_key(event, function);
}
} // namespace input
} // namespace cs
