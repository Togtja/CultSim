#include "input_handler.h"
#include "filesystem/filesystem.h"

#include <algorithm>
#include <string>
#include <string_view>

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

const robin_hood::unordered_map<SDL_Scancode, EAction>& ActionHandler::get_key_binding() const
{
    return m_key_binding;
}

const robin_hood::unordered_map<EMouse, EAction>& ActionHandler::get_mouse_binding() const
{
    return m_mouse_binding;
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

void ActionHandler::bind_btn(const EMouse button, const EAction action)
{
    m_mouse_binding[button] = action;
}

void ActionHandler::unbind_action(const EAction action)
{
    const auto it  = m_action_binding.find(action);
    const auto it2 = m_live_action_binding.find(action);
    if (it == m_action_binding.end() && it2 == m_live_action_binding.end())
    {
        spdlog::get("input")->trace("unbinding action {} for context id {}, not possible it does not exist",
                                    action,
                                    m_context_type);
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
    const auto it = m_key_binding.find(scancode);
    if (it == m_key_binding.end())
    {
        spdlog::get("input")->trace("unbinding key {} for context id {}, not possible it does not exist",
                                    get_key_name(scancode),
                                    m_context_type);
        return;
    }
    m_key_binding.erase(it);
}

void ActionHandler::unbind_btn(const EMouse button)
{
    const auto it = m_mouse_binding.find(button);
    if (it == m_mouse_binding.end())
    {
        spdlog::get("input")->trace("unbinding mouse button id {} for context id {}, not possible it does not exist",
                                    button,
                                    m_context_type);
        return;
    }
    m_mouse_binding.erase(it);
}

bool ActionHandler::handle_input(const SDL_Scancode scancode)
{
    if (has_event(scancode))
    {
        /** Call the function the scancode maps to */
        const auto& action = m_key_binding[scancode];
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
    /**
     * Used to make sure we break from stack after a key is found in a context
     * The other handles can just return, however this one needs to loop through all the keys
     * To keep the ability to hold down two keys at the same time
     */
    bool found_key       = false;
    const auto key_state = SDL_GetKeyboardState(nullptr);
    for (auto&& [k, a] : m_key_binding)
    {
        if (key_state[k])
        {
            if (has_live_action(a))
            {
                m_live_action_binding[a](dt);
                found_key = true;
            }
        }
    }
    const auto mouse_state = SDL_GetMouseState(nullptr, nullptr);
    for (auto&& [b, a] : m_mouse_binding)
    {
        /** If our enum is not apart of the SDL_Button's continue */
        if (static_cast<int>(b) > SDL_BUTTON_X2)
        {
            continue;
        }
        if (mouse_state & SDL_BUTTON(static_cast<int>(b)))
        {
            if (has_live_action(a))
            {
                m_live_action_binding[a](dt);
                found_key = true;
            }
        }
    }

    return m_blocking || found_key;
}

bool ActionHandler::handle_input(const EMouse button)
{
    if (has_event(button))
    {
        /** Call the function the button maps to */
        const auto& action = m_mouse_binding[button];
        if (has_action(action))
        {
            m_action_binding[action]();
            return true;
        }
    }
    return m_blocking;
}

bool ActionHandler::has_event(const SDL_Scancode scancode)
{
    return m_key_binding.contains(scancode);
}

bool ActionHandler::has_event(const EMouse button)
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
    clear_key_action();
    clear_action_func();
}

void ActionHandler::clear_key_action()
{
    m_key_binding.clear();
    m_mouse_binding.clear();
}

void ActionHandler::clear_action_func()
{
    m_action_binding.clear();
    m_live_action_binding.clear();
}

std::string ActionHandler::key_bindings_to_lua()
{
    std::string ret{};
    for (auto&& [k, a] : m_key_binding)
    {
        ret += fmt::format("\t\t[\"{}\"] = EAction.{},\n", SDL_GetScancodeName(k), action_to_string(a));
    }
    return ret;
}

std::string ActionHandler::btn_bindings_to_lua()
{
    std::string ret{};
    for (auto&& [b, a] : m_mouse_binding)
    {
        ret += fmt::format("\t\t[EMouse.{}] = EAction.{},\n", mouse_to_string(b), action_to_string(a));
    }
    return ret;
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
        spdlog::get("input")->warn("can not add default context on top of something else");
        return;
    }

    /** If it is not at the end then it is already there so remove it and then add it */
    const auto context_it = std::find(std::begin(m_active_stack), std::end(m_active_stack), context);
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
        spdlog::get("input")->warn("trying to remove the default context");
        return;
    }

    /** If it is not the end then we have it */
    auto context_it = std::find(std::begin(m_active_stack), std::end(m_active_stack), context);
    if (context_it != std::end(m_active_stack))
    {
        m_active_stack.erase(context_it);
    }
    else
    {
        spdlog::get("input")->trace("removing context id {} not possible it does not exist", context);
    }
}

void ContextHandler::pop_context()
{
    if (m_active_stack.size() <= 1)
    {
        spdlog::get("input")->warn("trying to remove the default context");
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
                                   const EMouse button,
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

void ContextHandler::bind_btn(const EKeyContext context, const EMouse button, const EAction action)
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

void ContextHandler::unbind_btn(const EKeyContext context, const EMouse button)
{
    if (has_context(context))
    {
        m_input_map.at(context).unbind_btn(button);
    }
}

void ContextHandler::handle_input(const SDL_Event& event)
{
    const auto& io = ImGui::GetIO();

    for (auto it = m_active_stack.crbegin(); it != m_active_stack.crend(); it++)
    {
        /** Handled the input */
        if (event.type == SDL_KEYDOWN)
        {
            if (!(io.WantCaptureKeyboard || io.WantTextInput))
            {
                if (m_input_map.at(*it).handle_input(event.key.keysym.scancode))
                {
                    return;
                }
            }
        }

        else if (event.type == SDL_MOUSEBUTTONDOWN && !io.WantCaptureMouse)
        {
            /** Translate from SDL Mouse Enum to our Mouse enum */
            const auto click = static_cast<EMouse>(event.button.button);

            /** Update last mouse positions */
            m_mouse_click_pos = {event.button.x, event.button.y};
            if (click == EMouse::BtnLeft)
            {
                m_left_click_pos = m_mouse_click_pos;
            }
            else if (click == EMouse::BtnRight)
            {
                m_right_click_pos = m_mouse_click_pos;
            }

            if (m_input_map.at(*it).handle_input(click))
            {
                return;
            }
        }
        else if (event.type == SDL_MOUSEWHEEL)
        {
            auto block = false;
            auto x     = event.wheel.x;
            auto y     = event.wheel.y;

            if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
            {
                x *= -1;
                y *= -1;
            }

            if (x > 0)
            {
                block |= m_input_map.at(*it).handle_input(EMouse::BtnWheelRight);
            }
            else if (x < 0)
            {
                block |= m_input_map.at(*it).handle_input(EMouse::BtnWheelLeft);
            }
            if (y > 0)
            {
                block |= m_input_map.at(*it).handle_input(EMouse::BtnWheelUp);
            }
            else if (y < 0)
            {
                block |= m_input_map.at(*it).handle_input(EMouse::BtnWheelDown);
            }

            if (block)
            {
                return;
            }
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
            mouse_pos = {event.motion.x, event.motion.y};
            if (m_input_map.at(*it).handle_input(EMouse::BtnMove))
            {
                return;
            }
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
        spdlog::get("input")->trace("clearing context {}, however it does not exist in the mapping", context);
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

    /** There always need to be a DefaultContext at the bottom of the stack */
    add_context(EKeyContext::DefaultContext);
}

const robin_hood::unordered_map<EKeyContext, detail::ActionHandler>& ContextHandler::get_input_map() const
{
    return m_input_map;
}

void ContextHandler::set_input_map(const robin_hood::unordered_map<EKeyContext, detail::ActionHandler>& map)
{
    m_input_map = map;
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
void ContextHandler::save_binding_to_file()
{
    std::stringstream lua_key_script;
    lua_key_script << "action_key_bindings = {\n";
    for (auto&& [context, action_h] : m_input_map)
    {
        lua_key_script << fmt::format("\t[EKeyContext.{}] = {{\n", key_context_to_string(context));
        lua_key_script << action_h.key_bindings_to_lua();
        lua_key_script << "\t},\n";
    }

    lua_key_script << "}\naction_mouse_bindings = {\n";
    for (auto&& [context, action_h] : m_input_map)
    {
        lua_key_script << fmt::format("\t[EKeyContext.{}] = {{\n", key_context_to_string(context));
        lua_key_script << action_h.btn_bindings_to_lua();
        lua_key_script << "\t},\n";
    }
    lua_key_script << "}";
    fs::write_file("keybinding.lua", lua_key_script.str());
}

void ContextHandler::load_binding_from_file(sol::state_view lua)
{
    const auto& file = fs::read_file("keybinding.lua");
    if (file.empty())
    {
        spdlog::get("input")->critical("failed to find key bindings, program running witout any key bindings");
        return;
    }
    lua.script(file);
    sol::table key_bindings = lua["action_key_bindings"];
    for (auto&& [context, key_action] : key_bindings)
    {
        for (auto&& [k, a] : key_action.as<sol::table>())
        {
            auto scancode = SDL_GetScancodeFromName(k.as<std::string>().c_str());
            spdlog::get("input")->debug("trying to bind {} to {} in {}",
                                        SDL_GetScancodeName(scancode),
                                        action_to_string(a.as<EAction>()),
                                        key_context_to_string(context.as<EKeyContext>()));

            if (scancode == SDL_SCANCODE_UNKNOWN)
            {
                spdlog::get("input")->warn("invalid key name {} when reading key bindings", k.as<std::string>());
                continue;
            }
            bind_key(context.as<EKeyContext>(), scancode, a.as<EAction>());
        }
    }

    sol::table mouse_bindings = lua["action_mouse_bindings"];
    for (auto&& [context, btn_action] : mouse_bindings)
    {
        for (auto&& [b, a] : btn_action.as<sol::table>())
        {
            spdlog::get("input")->trace("trying to bind {} to {} in {}",
                                        b.as<EMouse>(),
                                        a.as<EAction>(),
                                        context.as<EKeyContext>());
            bind_btn(context.as<EKeyContext>(), b.as<EMouse>(), a.as<EAction>());
        }
    }
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
