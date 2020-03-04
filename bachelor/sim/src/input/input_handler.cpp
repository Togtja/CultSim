#include "input_handler.h"

#include <algorithm>

#include <gfx/ImGUI/imgui.h>
#include <spdlog/spdlog.h>

namespace cs::input
{
namespace detail
{
ActionHandler::ActionHandler(const EKeyContext type)
{
    m_context_type = type;
}

void ActionHandler::set_blocking(const bool blocking)
{
    m_blocking = blocking;
}

void ActionHandler::bind_action(const EAction action, const std::function<void()>& function)
{
    m_action_binding[action] = function;
}

void ActionHandler::bind_action(const EAction action, const std::function<void(float)>& function)
{
    m_live_action_binding[action] = function;
}

void ActionHandler::bind_key(const SDL_Scancode scancode, const EAction action)
{
    m_key_binding[scancode] = action;
}

void ActionHandler::bind_btn(const Mouse button, const EAction action)
{
    m_mouse_binding[button] = action;
}

void ActionHandler::unbind_action(const EAction action)
{
    auto&& it  = m_action_binding.find(action);
    auto&& it2 = m_live_action_binding.find(action);
    if (it == m_action_binding.end() && it2 == m_live_action_binding.end())
    {
        spdlog::trace("unbinding action {} for context id {}, not possible it does not exist", action, m_context_type);
        return;
    }

    if (it != m_action_binding.end())
    {
        m_action_binding.erase(it);
    }
    if (it2 != m_live_action_binding.end())
    {
        m_live_action_binding.erase(it2);
    }
}

void ActionHandler::unbind_key(const SDL_Scancode scancode)
{
    auto&& it = m_key_binding.find(scancode);
    if (it == m_key_binding.end())
    {
        spdlog::trace("unbinding key {} for context id {}, not possible it does not exist",
                      get_key_name(scancode),
                      m_context_type);
        return;
    }
    m_key_binding.erase(it);
}

void ActionHandler::unbind_btn(const Mouse button)
{
    auto&& it = m_mouse_binding.find(button);
    if (it == m_mouse_binding.end())
    {
        spdlog::trace("unbinding mouse button id {} for context id {}, not possible it does not exist", button, m_context_type);
        return;
    }
    m_mouse_binding.erase(it);
}

bool ActionHandler::handle_input(const SDL_Scancode scancode)
{
    if (has_event(scancode))
    {
        // Call the function the scancode maps to
        auto& action = m_key_binding[scancode];
        if (has_action(action))
        {
            m_action_binding[action]();
            return true;
        }
    }
    return m_blocking;
}

bool ActionHandler::handle_live_input(const float dt)
{
    // Used to make sure we break from stack after a key is found in a context
    // The other handles can just return, however this one needs to loop through all the keys
    // To keep the ability to hold down two keys at the same time
    bool found_key = false;
    auto key_state = SDL_GetKeyboardState(nullptr);
    for (auto&& [k, v] : m_key_binding)
    {
        if (key_state[k])
        {
            if (has_live_action(v))
            {
                m_live_action_binding[v](dt);
                found_key = true;
            }
        }
    }
    return m_blocking || found_key;
}

bool ActionHandler::handle_input(const Mouse button)
{
    if (has_event(button))
    {
        // Call the function the button maps to
        auto& action = m_mouse_binding[button];
        m_action_binding[action]();
        return true;
    }
    return m_blocking;
}

bool ActionHandler::has_event(const SDL_Scancode scancode)
{
    return m_key_binding.contains(scancode);
}

bool ActionHandler::has_event(const Mouse button)
{
    return m_mouse_binding.contains(button);
}

bool ActionHandler::has_action(const EAction action)
{
    return m_action_binding.contains(action);
}

bool ActionHandler::has_live_action(const EAction action)
{
    return m_live_action_binding.contains(action);
}

void ActionHandler::clear()
{
    m_action_binding.clear();
    m_key_binding.clear();
    m_mouse_binding.clear();
}

std::string ActionHandler::get_key_name(SDL_Scancode scancode)
{
    return SDL_GetKeyName(SDL_GetKeyFromScancode(scancode));
}

} // namespace detail

/******** CONTEXT HANDLER *********/

void ContextHandler::add_context(const EKeyContext context, bool blocking)
{
    if (!m_active_stack.empty() && context == EKeyContext::DefaultContext)
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

void ContextHandler::remove_context(const EKeyContext context)
{
    if (context == EKeyContext::DefaultContext)
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
        spdlog::trace("removing context id {} not possible it does not exist", context);
    }
}

void ContextHandler::pop_context()
{
    if (m_active_stack.size() <= 1)
    {
        spdlog::warn("trying to remove the default context");
        return;
    }
    m_active_stack.pop_back();
}

void ContextHandler::fast_bind_key(const EKeyContext context,
                                   const SDL_Scancode scancode,
                                   const EAction action,
                                   const std::function<void()>& function)
{
    bind_key(context, scancode, action);
    bind_action(context, action, function);
}

void ContextHandler::fast_bind_btn(const EKeyContext context,
                                   const Mouse button,
                                   const EAction action,
                                   const std::function<void()>& function)
{
    bind_btn(context, button, action);
    bind_action(context, action, function);
}

void ContextHandler::bind_action(EKeyContext context, const EAction action, const std::function<void()>& function)
{
    get_action_handler(context).bind_action(action, function);
}

void ContextHandler::bind_action(EKeyContext context, const EAction action, const std::function<void(float)>& function)
{
    get_action_handler(context).bind_action(action, function);
}

void ContextHandler::bind_key(const EKeyContext context, const SDL_Scancode scancode, const EAction action)
{
    get_action_handler(context).bind_key(scancode, action);
}

void ContextHandler::bind_btn(const EKeyContext context, const Mouse button, const EAction action)
{
    get_action_handler(context).bind_btn(button, action);
}

void ContextHandler::unbind_action(const EKeyContext context, const EAction action)
{
    if (has_context(context))
    {
        m_input_map.at(context).unbind_action(action);
    }
}

void ContextHandler::unbind_key(const EKeyContext context, const SDL_Scancode scancode)
{
    if (has_context(context))
    {
        m_input_map.at(context).unbind_key(scancode);
    }
}

void ContextHandler::unbind_btn(const EKeyContext context, const Mouse button)
{
    if (has_context(context))
    {
        m_input_map.at(context).unbind_btn(button);
    }
}

void ContextHandler::handle_input(const SDL_Event& event)
{
    bool break_event = false;
    for (auto it = m_active_stack.crbegin(); it != m_active_stack.crend(); it++)
    {
        // Handled the input
        if (event.type == SDL_KEYDOWN)
        {
            if (m_input_map.at(*it).handle_input(event.key.keysym.scancode))
            {
                break_event = true;
            }
        }
        if (event.type == SDL_MOUSEBUTTONDOWN && !ImGui::GetIO().WantCaptureMouse)
        {
            // Subtract 1 to translate from SDL Mouse Enum to our Mouse enum
            auto click = static_cast<Mouse>(event.button.button - 1);
            // Update last mouse positions
            m_mouse_click_pos = {event.button.x, event.button.y};
            if (click == Mouse::Left)
            {
                m_left_click_pos = m_mouse_click_pos;
            }
            if (click == Mouse::Right)
            {
                m_right_click_pos = m_mouse_click_pos;
            }
            if (m_input_map.at(*it).handle_input(click))
            {
                break_event = true;
            }
        }
        if (event.type == SDL_MOUSEWHEEL)
        {
            auto x = event.wheel.x;
            auto y = event.wheel.y;
            if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
            {
                x *= -1;
                y *= -1;
            }
            if (x > 0)
            {
                if (m_input_map.at(*it).handle_input(Mouse::WheelRight))
                {
                    break_event = true;
                }
            }
            if (x < 0)
            {
                if (m_input_map.at(*it).handle_input(Mouse::WheelLeft))
                {
                    break_event = true;
                }
            }
            if (y > 0)
            {
                if (m_input_map.at(*it).handle_input(Mouse::WheelUp))
                {
                    break_event = true;
                }
            }
            if (y < 0)
            {
                if (m_input_map.at(*it).handle_input(Mouse::WheelDown))
                {
                    break_event = true;
                }
            }
        }
        if (event.type == SDL_MOUSEMOTION)
        {
            mouse_pos = {event.motion.x, event.motion.y};
            if (m_input_map.at(*it).handle_input(Mouse::Move))
            {
                break_event = true;
            }
        }
        if (break_event)
        {
            return;
        }
    }
}

void ContextHandler::handle_live_input(float dt)
{
    for (auto it = m_active_stack.crbegin(); it != m_active_stack.crend(); it++)
    {
        if (has_context(*it))
        {
            if (m_input_map.at(*it).handle_live_input(dt))
            {
                return;
            }
        }
    }
}

bool ContextHandler::has_context(const EKeyContext context)
{
    return m_input_map.contains(context);
}

void ContextHandler::clear_context(const EKeyContext context)
{
    if (has_context(context))
    {
        m_input_map.at(context).clear();
    }
    else
    {
        spdlog::trace("clearing context {}, however it does not exist in the mapping", context);
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
    add_context(EKeyContext::DefaultContext);
}

glm::ivec2 ContextHandler::get_mouse_click_pos()
{
    return m_mouse_click_pos;
}

glm::ivec2 ContextHandler::get_mouse_rclick_pos()
{
    return m_right_click_pos;
}

glm::ivec2 ContextHandler::get_mouse_lclick_pos()
{
    return m_left_click_pos;
}

glm::ivec2 ContextHandler::get_mouse_pos()
{
    return mouse_pos;
}

ContextHandler::ContextHandler()
{
    add_context(EKeyContext::DefaultContext);
}

detail::ActionHandler& ContextHandler::get_action_handler(EKeyContext context)
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
