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
