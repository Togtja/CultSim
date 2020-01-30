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
InputHandler::~InputHandler()
{
}

/******** CONTEXT HANDLER *********/
ContextHandler::ContextHandler()
{
}
void ContextHandler::bind_key(KeyContext context, const SDL_Scancode event, const std::function<void()> function)
{
    m_input_map[context].bind_key(event, function);
}
} // namespace input
} // namespace cs
