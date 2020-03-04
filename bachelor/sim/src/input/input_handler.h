#pragma once
#include <functional>
#include <vector>

#include "robin_hood.h"
#include <SDL_events.h>
#include <glm/vec2.hpp>
namespace cs::input
{
/**
 * KeyContext
 * the context in which the program is in, with a certain key mapping.
 */
enum class EKeyContext
{
    DefaultContext,
    Agent,
    AgentOnHover,
    ScenarioScene
};

/**
 * Action, the step between a key and a functions
 * A key (Keyboard key/MouseBtn) gets mapped to an Action
 * And an Action get mapped to a certain function
 */
enum class Action
{
    MoveUp,
    MoveLeft,
    MoveRight,
    MoveDown,
    ZoomIn,
    ZoomOut,
    Pause,
    SelectEntity,
    FollowEntity
};

/**
 * Mouse, a easier way to use the diffrent SDL_events
 */
enum class Mouse
{
    Left,       // Left mouse btn
    Middle,     // Middle mouse btn
    Right,      // Right mouse btn
    X1,         // Optional mouse btn 1
    X2,         // Optional mouse btn 2
    WheelUp,    // Scroll wheel up
    WheelDown,  // Scroll wheel down
    WheelRight, // Scroll wheel right
    WheelLeft,  // Scroll wheel left
    Move        // Any mouse movement

};
namespace detail
{
/** Handles Actions for a certain context */
class ActionHandler
{
private:
    EKeyContext m_context_type;
    // Action to function bindings
    robin_hood::unordered_map<Action, std::function<void()>> m_action_binding;
    robin_hood::unordered_map<Action, std::function<void(float)>> m_live_action_binding;

    // Key to Action Binding
    robin_hood::unordered_map<SDL_Scancode, Action> m_key_binding;
    robin_hood::unordered_map<Mouse, Action> m_mouse_binding;

    // If blocking it will not go further down the context stack for keys
    bool m_blocking = false;

public:
    /**
     * Handles an Action for a certain context
     *
     * @param type The Key Context that the input handler should handle
     */
    explicit ActionHandler(const EKeyContext type);

    void set_blocking(const bool blocking);

    /**
     * Given an Action and a function binds that Action to that function
     *
     * @note many to 1 binding (ie. actions can do the same functions)
     *
     * @param action The Action enum you want to bind
     * @param function The function you want to trigger when the action gets triggered
     */
    void bind_action(const Action action, const std::function<void()>& function);

    /**
     * Given an Action and a function binds that Action to that function
     *
     *  @note many to 1 binding (ie. actions can do the same functions)
     * @note Lambda function takes in a float that is delta time
     *
     * @param action The Action enum you want to bind
     * @param function The function you want to trigger when the action gets triggered
     */
    void bind_action(const Action action, const std::function<void(float)>& function);

    /**
     * Given an key (scancode) and an action, bind them together
     *
     * @note many to 1 binding
     *
     * @param scancode the SDL scancode of the key you want to trigger an action with
     * @param action the Action you want to trigger with the given scancode
     */
    void bind_key(const SDL_Scancode scancode, const Action action);

    /**
     * Given an Mouse button and an action, bind them together
     *
     * @note many to 1 binding
     *
     * @param button the Mouse button from the Mouse enum you want to trigger an action with
     * @param action the Action you want to trigger with the given button
     */
    void bind_btn(const Mouse button, const Action action);

    /**
     * Unbinds an action from the function
     *
     * @note unbind both
     *
     * @param action The action you want to unbind
     */
    void unbind_action(const Action action);

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
    void unbind_btn(const Mouse button);

    /**
     * Given a scancode (key), triggers the action it's binded to
     * Then the action triggers the function
     *
     * @param scancode the key even you want to trigger
     *
     * @return true if we don't need to go further down the context stack
     */
    bool handle_input(const SDL_Scancode scancode);

    /**
     * Given a Mouse button, triggers the action it's binded to
     * Then the action triggers the function
     *
     * @param scancode the key even you want to trigger
     *
     * @return true if we don't need to go further down the context stack
     */
    bool handle_input(const Mouse button);

    /**
     * Every frame checks the keys connected to a live action
     * And runs that function with delta time as parameter
     *
     * @param dt the delta time (time between frames)
     */
    bool handle_live_input(const float dt);

    /**
     * Given an scancode, checks if this context has that scancode
     *
     * @param scancode The scancode you want to check
     *
     * @return true if the scancode is in the context else false
     */
    bool has_event(const SDL_Scancode scancode);

    /**
     * Given an mousebutton, checks if this context has that mousebutton
     *
     * @param button The mousebutton you want to check
     *
     * @return true if the mousebutton is in the context else false
     */
    bool has_event(const Mouse button);

    /**
     * Given an action, checks if this context has that action
     *
     * @param action The action you want to check
     *
     * @return true if the action is in the context else false
     */
    bool has_action(const Action action);
    /**
     * Given a live action, checks if this context has that action
     *
     * @param action The live action you want to check
     *
     * @return true if the live action is in the context else false
     */
    bool has_live_action(const Action action);

    /**
     * Clears the key bindings
     */
    void clear();

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
     * Adds a new context and create an action handler for it
     * if it does not already exist, and add it to the top of the context stack
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
     * @param context the context you want it all to be binded to
     * @param scancode the SDL scancode to bind to the action
     * @param action the Action to bind to the function
     * @param function the function you want to run when the scancode is pressed
     */
    void fast_bind_key(const EKeyContext context,
                       const SDL_Scancode scancode,
                       const Action action,
                       const std::function<void()>& function);

    /**
     * A quicker way to bind a mousebutton to an action and a action to a function
     *
     * @note Only use for first time binding
     *
     * @param context the context you want it all to be binded to
     * @param button the mousebutton to bind to the action
     * @param action the Action to bind to the function
     * @param function the function you want to run when the scancode is pressed
     */
    void fast_bind_btn(const EKeyContext context, const Mouse button, const Action action, const std::function<void()>& function);

    /**
     * Bind context to an action-function
     *
     * @param context The context you want to bind an action-function to
     * @param action The action you want to bind to the context
     * @param function The function you want you bind to the action
     */
    void bind_action(const EKeyContext context, const Action action, const std::function<void()>& function);

    /**
     * Bind context to an action-function
     *
     * @note function takes a float (dt) parameter
     *
     * @param context The context you want to bind an action-function to
     * @param action The action you want to bind to the context
     * @param function The function you want you bind to the action
     */
    void bind_action(const EKeyContext context, const Action action, const std::function<void(float)>& function);

    /**
     * Bind context to a key-action
     *
     * @param context The context you want to bind a key-action
     * @param scancode The key you want to bind to action
     * @param action The action you want you bind to the key
     */
    void bind_key(const EKeyContext context, const SDL_Scancode scancode, const Action action);

    /**
     * Bind context to a mousebutton-action
     *
     * @param context The context you want to bind a mousebutton-action
     * @param button The mousebutton you want to bind to action
     * @param action The action you want you bind to the mousebutton
     */
    void bind_btn(const EKeyContext context, const Mouse button, const Action action);

    /**
     * Unbind a action-function from a context
     *
     * @param context The context you want to unbind from
     * @param action The action you want to unbind from the context and its function
     */
    void unbind_action(const EKeyContext context, const Action action);

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
    void unbind_btn(const EKeyContext context, const Mouse button);

    /**
     * Handle's input from an event, goes through the context stack and runs the first found matching event
     *
     * @param event The event you want to run
     */
    void handle_input(const SDL_Event& event);

    /**
     * Handle's input every frame, goes through the context stack and runs the first match
     * matching is based on what key you are holding down
     *
     * @param event The event you want to run
     */
    void handle_live_input(const float dt);

    /**
     * Checks if the context have a mapping to an InputHandler
     *
     * @param KeyContext to check if exist in the mapping
     *
     * @return true if the context exist in the map, else false
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
     * @note if mouse is outside of program it will find where it was last found
     *
     * @return Position of the mouse
     */
    glm::ivec2 get_mouse_pos();

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
