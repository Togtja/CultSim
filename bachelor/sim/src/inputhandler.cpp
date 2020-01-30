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

} // namespace input
} // namespace cs
