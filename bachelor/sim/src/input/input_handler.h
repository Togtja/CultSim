#pragma once

#include <functional>
#include <vector>

#include <SDL_events.h>
#include <glm/vec2.hpp>
#include <robin_hood.h>
#include <sol/sol.hpp>

namespace cs::input
{
/**
 * KeyContext
 * the context in which the program is in, with a certain key mapping.
 */
enum class EKeyContext
{
    None, // No KeyContext set
    DefaultContext,
    Agent,
    AgentOnHover,
    ScenarioScene,
    PauseMenu,
    PreferenceScene,
    EditorScene,
    LoadScenario,
    RenderingSystem,

    Count
};

/**
 * Converts an EKeyContext Enum to string of its name
 *
 * @param context the EKeyContext you want to convert
 *
 * @return the string name of the context you sent as a parameter
 */
inline std::string key_context_to_string(const EKeyContext context)
{
    switch (context)
    {
        case EKeyContext::None: return "None";
        case EKeyContext::DefaultContext: return "DefaultContext";
        case EKeyContext::Agent: return "Agent";
        case EKeyContext::AgentOnHover: return "AgentOnHover";
        case EKeyContext::ScenarioScene: return "ScenarioScene";
        case EKeyContext::PauseMenu: return "PauseMenu";
        case EKeyContext::PreferenceScene: return "PreferenceScene";
        case EKeyContext::EditorScene: return "EditorScene";
        case EKeyContext::LoadScenario: return "LoadScenario";
        case EKeyContext::RenderingSystem: return "RenderingSystem";

        default: return "Error: Invalid Context (Consider adding it to_string)";
    }
}

/**
 * Action, the step between a key and a functions
 * A key (Keyboard key/MouseBtn) gets mapped to an Action
 * And an Action get mapped to a certain function
 */
enum class EAction
{
    None = 0,
    MoveUp,
    MoveLeft,
    MoveRight,
    MoveDown,
    ZoomIn,
    ZoomOut,
    PauseMenu,
    SelectEntity,
    FollowEntity,
    SpeedUp,
    SpeedDown,
    Pause,
    Quit,
    EscapeScene,
    ReloadShaders,
    SetMode2D,
    SetMode3D,

    Count
};

/**
 * Converts an EAction Enum to string of its name
 *
 * @param action the EAction you want to convert
 *
 * @return the string name of the action you sent as a parameter
 */
inline std::string action_to_string(const EAction action)
{
    switch (action)
    {
        case EAction::None: return "None";
        case EAction::MoveUp: return "MoveUp";
        case EAction::MoveLeft: return "MoveLeft";
        case EAction::MoveRight: return "MoveRight";
        case EAction::MoveDown: return "MoveDown";
        case EAction::ZoomIn: return "ZoomIn";
        case EAction::ZoomOut: return "ZoomOut";
        case EAction::PauseMenu: return "PauseMenu";
        case EAction::SelectEntity: return "SelectEntity";
        case EAction::FollowEntity: return "FollowEntity";
        case EAction::SpeedUp: return "SpeedUp";
        case EAction::SpeedDown: return "SpeedDown";
        case EAction::Pause: return "Pause";
        case EAction::Quit: return "Quit";
        case EAction::EscapeScene: return "EscapeScene";
        case EAction::ReloadShaders: return "ReloadShaders";
        case EAction::SetMode2D: return "SetMode2D";
        case EAction::SetMode3D: return "SetMode3D";

        default: return "Action not added to to_string";
    }
}

/**
 * Mouse, a easier way to use the diffrent SDL_events
 */
enum class EMouse
{
    BtnNone = 0,   // No Press
    BtnLeft,       // Left mouse btn
    BtnMiddle,     // Middle mouse btn
    BtnRight,      // Right mouse btn
    BtnX1,         // Optional mouse btn 1
    BtnX2,         // Optional mouse btn 2
    BtnWheelUp,    // Scroll wheel up
    BtnWheelDown,  // Scroll wheel down
    BtnWheelRight, // Scroll wheel right
    BtnWheelLeft,  // Scroll wheel left
    BtnMove,       // Any mouse movement

    Count

};

/**
 * Converts an EMouse Enum to string of its name
 *
 * @param btn the EMouse button you want to convert
 *
 * @return the string name of the button you sent as a parameter
 */
inline std::string mouse_to_string(const EMouse btn)
{
    switch (btn)
    {
        case EMouse::BtnLeft: return "BtnLeft";
        case EMouse::BtnMiddle: return "BtnMiddle";
        case EMouse::BtnRight: return "BtnRight";
        case EMouse::BtnX1: return "BtnX1";
        case EMouse::BtnX2: return "BtnX2";
        case EMouse::BtnWheelUp: return "BtnWheelUp";
        case EMouse::BtnWheelDown: return "BtnWheelDown";
        case EMouse::BtnWheelRight: return "BtnWheelRight";
        case EMouse::BtnWheelLeft: return "BtnWheelLeft";
        case EMouse::BtnMove: return "BtnMove";
        case EMouse::BtnNone: return "Error None Button";

        case EMouse::Count: return "Error Button";
    }
    return "Mouse not added to to_string";
}

inline EMouse string_to_mouse(std::string_view btn_name)
{
    if (btn_name == "BtnLeft")
    {
        return EMouse::BtnLeft;
    }
    else if (btn_name == "BtnMiddle")
    {
        return EMouse::BtnMiddle;
    }
    else if (btn_name == "BtnRight")
    {
        return EMouse::BtnRight;
    }
    else if (btn_name == "BtnX1")
    {
        return EMouse::BtnX1;
    }
    else if (btn_name == "BtnX2")
    {
        return EMouse::BtnX2;
    }
    else if (btn_name == "BtnWheelUp")
    {
        return EMouse::BtnWheelUp;
    }
    else if (btn_name == "BtnWheelDown")
    {
        return EMouse::BtnWheelDown;
    }
    else if (btn_name == "BtnWheelRight")
    {
        return EMouse::BtnWheelRight;
    }
    else if (btn_name == "BtnWheelLeft")
    {
        return EMouse::BtnWheelLeft;
    }
    else if (btn_name == "BtnMove")
    {
        return EMouse::BtnMove;
    }
    return EMouse::BtnNone;
}
namespace detail
{
/** Handles Actions for a certain context */
class ActionHandler
{
private:
    EKeyContext m_context_type;

    /** Action to function bindings */
    robin_hood::unordered_map<EAction, std::function<void()>> m_action_binding{};
    robin_hood::unordered_map<EAction, std::function<void(float)>> m_live_action_binding{};

    /** Key to Action Binding */
    robin_hood::unordered_map<SDL_Scancode, EAction> m_key_binding{};
    robin_hood::unordered_map<EMouse, EAction> m_mouse_binding{};

    /** If blocking it will not go further down the context stack for keys */
    bool m_blocking{false};

public:
    /**
     * Handles an Action for a certain context
     *
     * @param type The Key Context that the input handler should handle
     */
    explicit ActionHandler(const EKeyContext type);

    /**
     * Get the Action handler's key binding
     *
     * @return The key binding unsorted map
     */
    const robin_hood::unordered_map<SDL_Scancode, EAction>& get_key_binding() const;

    /**
     * Get the Action handler's mouse binding
     *
     * @return The mouse binding unsorted map
     */
    const robin_hood::unordered_map<EMouse, EAction>& get_mouse_binding() const;

    /**
     * Set if the ActionHandler should block the stack search for keybindings
     *
     * @note Default is initialized to non-blocking
     *
     * @param blocking True if you want the ActionHandler to be blocking
     */
    void set_blocking(const bool blocking);

    /**
     * Given an Action and a function binds that Action to that function
     *
     * @note Many to 1 binding (ie. actions can do the same functions)
     *
     * @param action The Action enum you want to bind
     * @param function The function you want to trigger when the action gets triggered
     */
    void bind_action(const EAction action, const std::function<void()>& function);

    /**
     * Given an Action and a function binds that Action to that function
     *
     * @note Many to 1 binding (ie. actions can do the same functions)
     * @note Lambda function takes in a float that is delta time
     *
     * @param action The Action enum you want to bind
     * @param function The function you want to trigger when the action gets triggered
     */
    void bind_action(const EAction action, const std::function<void(float)>& function);

    /**
     * Given an key (scancode) and an action, bind them together
     *
     * @note Many to 1 binding
     *
     * @param scancode The SDL scancode of the key you want to trigger an action with
     * @param action The Action you want to trigger with the given scancode
     */
    void bind_key(const SDL_Scancode scancode, const EAction action);

    /**
     * Given an Mouse button and an action, bind them together
     *
     * @note Many to 1 binding
     *
     * @param button The Mouse button from the Mouse enum you want to trigger an action with
     * @param action The Action you want to trigger with the given button
     */
    void bind_btn(const EMouse button, const EAction action);

    /**
     * Unbinds an action from the function
     *
     * @note Unbind both
     *
     * @param action The action you want to unbind
     */
    void unbind_action(const EAction action);

    /**
     * Unbinds a key from the action
     *
     * @param scancode The key you want to unbind
     */
    void unbind_key(const SDL_Scancode scancode);

    /**
     * Unbinds a Mouse button from the action
     *
     * @param button The key you want to unbind
     */
    void unbind_btn(const EMouse button);

    /**
     * Given a scancode (key), triggers the action it's bound to, then the action triggers the function
     *
     * @param scancode The key even you want to trigger
     *
     * @return True if we don't need to go further down the context stack
     */
    bool handle_input(const SDL_Scancode scancode);

    /**
     * Given a Mouse button, triggers the action it's bound to, then the action triggers the function
     *
     * @param scancode The key even you want to trigger
     *
     * @return True if we don't need to go further down the context stack
     */
    bool handle_input(const EMouse button);

    /**
     * Every frame checks the keys connected to a live action, and runs that function with delta time as parameter
     *
     * @param dt The delta time (time between frames)
     */
    bool handle_live_input(const float dt);

    /**
     * Given an scancode, checks if this context has that scancode
     *
     * @param scancode The scancode you want to check
     *
     * @return True if the scancode is in the context else false
     */
    bool has_event(const SDL_Scancode scancode);

    /**
     * Given an mousebutton, checks if this context has that mousebutton
     *
     * @param button The mousebutton you want to check
     *
     * @return True if the mousebutton is in the context else false
     */
    bool has_event(const EMouse button);

    /**
     * Given an action, checks if this context has that action
     *
     * @param action The action you want to check
     *
     * @return True if the action is in the context else false
     */
    bool has_action(const EAction action);
    /**
     * Given a live action, checks if this context has that action
     *
     * @param action The live action you want to check
     *
     * @return True if the live action is in the context else false
     */
    bool has_live_action(const EAction action);

    /**
     * Clears the key bindings
     */
    void clear();

    void clear_key_action();
    void clear_action_func();

    std::string key_bindings_to_lua();
    std::string btn_bindings_to_lua();

private:
    std::string get_key_name(SDL_Scancode scancode);
};

} // namespace detail

class ContextHandler
{
private:
    std::vector<EKeyContext> m_active_stack;
    robin_hood::unordered_map<EKeyContext, detail::ActionHandler> m_input_map;

    glm::ivec2 m_mouse_click_pos{};
    glm::ivec2 m_right_click_pos{};
    glm::ivec2 m_left_click_pos{};
    glm::ivec2 mouse_pos{};

public:
    friend ContextHandler& get_input();
    ContextHandler(ContextHandler const&) = delete;
    void operator=(ContextHandler const&) = delete;

    /**
     * Add a new context and create an action handler for it, if it does not already exist, and add it to the top of the context
     * stack
     *
     * @param context The context you want to create an input handler for
     */
    void add_context(const EKeyContext context, bool blocking = false);

    /**
     * Remove a context for the context stack
     *
     * @param context The context you want the remove the input handler for
     */
    void remove_context(const EKeyContext context);

    /**
     * Remove the top context from the context stack
     */
    void pop_context();

    /**
     * A quicker way to bind a scancode to an action and a action to a function
     *
     * @note Only use for first time binding
     *
     * @param context The context you want it all to be bound to
     * @param scancode The SDL scancode to bind to the action
     * @param action The Action to bind to the function
     * @param function The function you want to run when the scancode is pressed
     */
    void fast_bind_key(const EKeyContext context,
                       const SDL_Scancode scancode,
                       const EAction action,
                       const std::function<void()>& function);

    /**
     * A quicker way to bind a mousebutton to an action and a action to a function
     *
     * @note Only use for first time binding
     *
     * @param context The context you want it all to be bound to
     * @param button The mousebutton to bind to the action
     * @param action The Action to bind to the function
     * @param function The function you want to run when the scancode is pressed
     */
    void
    fast_bind_btn(const EKeyContext context, const EMouse button, const EAction action, const std::function<void()>& function);

    /**
     * Bind context to an action-function
     *
     * @param context The context you want to bind an action-function to
     * @param action The action you want to bind to the context
     * @param function The function you want you bind to the action
     */
    void bind_action(const EKeyContext context, const EAction action, const std::function<void()>& function);

    /**
     * Bind context to an action-function
     *
     * @note Function takes a float (dt) parameter
     *
     * @param context The context you want to bind an action-function to
     * @param action The action you want to bind to the context
     * @param function The function you want you bind to the action
     */
    void bind_action(const EKeyContext context, const EAction action, const std::function<void(float)>& function);

    /**
     * Bind context to a key-action
     *
     * @param context The context you want to bind a key-action
     * @param scancode The key you want to bind to action
     * @param action The action you want you bind to the key
     */
    void bind_key(const EKeyContext context, const SDL_Scancode scancode, const EAction action);

    /**
     * Bind context to a mousebutton-action
     *
     * @param context The context you want to bind a mousebutton-action
     * @param button The mousebutton you want to bind to action
     * @param action The action you want you bind to the mousebutton
     */
    void bind_btn(const EKeyContext context, const EMouse button, const EAction action);

    /**
     * Unbind a action-function from a context
     *
     * @param context The context you want to unbind from
     * @param action The action you want to unbind from the context and its function
     */
    void unbind_action(const EKeyContext context, const EAction action);

    /**
     * Unbind a key-action from a context
     *
     * @param context The context you want to unbind from
     * @param scancode The key you want to unbind from the context and its action
     */
    void unbind_key(const EKeyContext context, const SDL_Scancode scancode);

    /**
     * Unbind a mousebutton-action from a context
     *
     * @param context The context you want to unbind from
     * @param button The mousebutton you want to unbind from the context and its action
     */
    void unbind_btn(const EKeyContext context, const EMouse button);

    /**
     * Handle's input from an event, goes through the context stack and runs the first found matching event
     *
     * @param event The event you want to run
     */
    void handle_input(const SDL_Event& event);

    /**
     * Handle's input every frame, goes through the context stack and runs the first match, matching is based on what key you are
     * holding down
     *
     * @param event The event you want to run
     */
    void handle_live_input(const float dt);

    /**
     * Checks if the context have a mapping to an InputHandler
     *
     * @param context To check if exist in the mapping
     *
     * @return True if the context exist in the map, else false
     */
    bool has_context(const EKeyContext context);

    /**
     * Clears all keybinding to a certain context
     */
    void clear_context(const EKeyContext context);

    /**
     * Similar to clear, except it keeps all the keybindings
     */
    void back_to_default();

    /**
     * Clears KeyContext stack, and the mappings
     */
    void clear();

    const robin_hood::unordered_map<EKeyContext, detail::ActionHandler>& get_input_map() const;

    void set_input_map(const robin_hood::unordered_map<EKeyContext, detail::ActionHandler>& map);

    /**
     * Get the last mouse click
     *
     * @return Position of mouse click
     */
    glm::ivec2 get_mouse_click_pos();

    /**
     * Get the last right-mouse click
     *
     * @return Position of mouse click
     */
    glm::ivec2 get_mouse_rclick_pos();

    /**
     * Get the last left-mouse click
     *
     * @return Position of mouse click
     */
    glm::ivec2 get_mouse_lclick_pos();

    /**
     * Get the mouse position within the program
     *
     * @note If mouse is outside of program it will find where it was last found
     *
     * @return Position of the mouse
     */
    glm::ivec2 get_mouse_pos();

    /**
     * Load the key binding from the lua file
     *
     * @param lua The state of lua
     */
    void load_binding_from_file(sol::state_view lua);
    void save_binding_to_file();

private:
    ContextHandler();
    detail::ActionHandler& get_action_handler(EKeyContext context);
};
/**
 * Get the input manager singleton
 *
 * @return The input manager singleton
 */
ContextHandler& get_input();
} // namespace cs::input
