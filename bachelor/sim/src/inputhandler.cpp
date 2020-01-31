#include "inputhandler.h"

#include <algorithm>

#include <spdlog/spdlog.h>

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

void ContextHandler::remove_context()
{
    m_active_stack.pop_back();
}

void ContextHandler::bind_key(KeyContext context, const SDL_Scancode event, const std::function<void()> function)
{
    auto input_it = m_input_map.find(context);
    if (input_it != m_input_map.end())
    {
        // Get by reference, right?
        auto input = m_input_map.at(context);
        input.bind_key(event, function);
    }
    else
    {
        InputHandler new_input{context};
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
        spdlog::debug("could not unbind, because no input created for the context (id: {}", context);
    }
}

void ContextHandler::handle_input(const SDL_Scancode event)
{
    // Iterate over the the active context stack
    for (auto it = m_active_stack.begin(); it != m_active_stack.end(); it++)
    {
        auto input_it = m_input_map.find(*it);
        if (input_it != m_input_map.end())
        {
            auto input = m_input_map.at(*it);
            input.handle_input(event);
            spdlog::debug("we found the input {}", event);
            return;
        }
    }
    spdlog::debug("could not find anything for the {} event", event);
}
} // namespace input
} // namespace cs
