#include <functional>
#include <vector>

#include "robin_hood.h"
#include <SDL_events.h>
namespace cs::input
{
enum class KeyContext
{
    DefaultContext,
    Agent,
    AgentOnHover,
    ScenarioScene
};
enum class Action
{
    MoveFWD,
    MoveLeft,
    MoveRight,
    MoveBack,
    ZoomIn,
    ZoomOut,
    Pause,
};
namespace detail
{
/** Handles input for a certain context */
class ActionHandler
{
private:
    robin_hood::unordered_map<Action, std::function<void()>> m_action_binding;
    robin_hood::unordered_map<Action, std::function<void(float)>> m_live_action_binding;

    robin_hood::unordered_map<SDL_Scancode, Action> m_key_binding;
    robin_hood::unordered_map<Uint8, Action> m_mouse_binding;
    KeyContext m_context_type;
    bool m_blocking = false;

public:
    /**
     * Handles an Action for a certain context
     *
     * @param type The Key Context that the input handler should handle
     */
    explicit ActionHandler(const KeyContext type);
    ~ActionHandler();

    void set_blocking(bool blocking);

    /**
     * Given a key and a function binds that key to that function in the input handlers context
     *
     * @param scancode The key code that you want to bind
     * @param function The function you want that key to be binded to
     */
    void bind_action(const Action action, const std::function<void()>& function);
    void bind_action(const Action action, const std::function<void(float)>& function);
    void bind_key(const SDL_Scancode scancode, const Action action);
    void bind_btn(const Uint8 button, const Action action);

    /**
     * Unbinds a key in the input handlers context
     *
     * @param scancode The key even you want to unbind
     */
    void unbind_action(const Action action);
    void unbind_key(const SDL_Scancode scancode);
    void unbind_btn(const Uint8 button);

    /**
     * Given a key, runs that key's binded function in the input handlers context
     *
     * @param scancode the key even you want to trigger
     */
    bool handle_input(const SDL_Scancode scancode);
    bool handle_live_input(const float dt);
    bool handle_input(const Uint8 button);

    /**
     * Given an Key code, checks if this context has that key code
     *
     * @param scancode The scancode you want to check
     *
     * @return true if the scancode is in the context else false
     */
    bool has_event(const SDL_Scancode scancode);

    bool has_event(const Uint8 button);

    bool has_action(const Action action);
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
    std::vector<KeyContext> m_active_stack;
    robin_hood::unordered_map<KeyContext, detail::ActionHandler> m_input_map;

    glm::ivec2 last_click{};
    glm::ivec2 last_right{};
    glm::ivec2 last_left{};
    glm::ivec2 last_move{};

public:
    friend ContextHandler& get_input();
    ContextHandler(ContextHandler const&) = delete;
    void operator=(ContextHandler const&) = delete;
    /**
     * Adds a new context and create an input handler for it
     * and add it to the top of the context stack
     *
     * @param context The context you want to create an input handler for
     */
    void add_context(const KeyContext context, bool blocking = false);

    /**
     * Remove a context and the input handler
     *
     * @param context The context you want the remove the input handler for
     */
    void remove_context(const KeyContext context);

    /**
     * Remove the top context from the context stack
     */
    void remove_context();

    void fast_bind_key(const KeyContext context,
                       const SDL_Scancode scancode,
                       const Action action,
                       const std::function<void()>& function);

    void fast_bind_btn(const KeyContext context, const Uint8 button, const Action action, const std::function<void()>& function);
    /**
     * Bind context to a key, and that key to a function
     *
     * @param context The context you want to bind a key for
     * @param scancode The key you want to bind to the context
     * @param function The function you want you bind to the key code
     */
    void bind_action(KeyContext context, const Action action, const std::function<void()>& function);
    void bind_action(KeyContext context, const Action action, const std::function<void(float)>& function);
    void bind_key(const KeyContext context, const SDL_Scancode scancode, const Action action);
    void bind_btn(const KeyContext context, Uint8 button, const Action action);

    /**
     * Unbind a key from a context
     *
     * @param context The context you want to unbind from
     * @param scancode The key you want to unbind from the context
     */
    void unbind_action(const KeyContext context, const Action action);
    void unbind_key(const KeyContext context, const SDL_Scancode scancode);
    void unbind_btn(const KeyContext context, const Uint8 button);
    /**
     * Handle's input from an event, goes through the context stack and runs the first found event matching function
     *
     * @param scancode The scancode you want to run
     */
    void handle_input(const SDL_Scancode scancode);
    void handle_live_input(const float dt);
    void handle_input(const Uint8 button);

    /**
     * Checks if the context have a mapping to an InputHandler
     *
     * @param KeyContext to check if exist in the mapping
     *
     * @return true if the context exist in the map, else false
     */
    bool has_context(const KeyContext context);

    /**
     * Clears all keybinding to a certain context
     */
    void clear_context(const KeyContext context);

    /**
     * Similar to clear, except it keeps all the keybindings
     */
    void back_to_default();

    /**
     * Clears KeyContext stack, and the mappings
     */
    void clear();

    glm::ivec2 get_last_click();
    glm::ivec2 get_last_right_click();
    glm::ivec2 get_last_left_click();
    glm::ivec2 get_last_move();

private:
    ContextHandler();
    detail::ActionHandler& get_action_handler(KeyContext context);
};
/**
 * Get the input manager singleton
 *
 * @return The input manager singleton
 */
ContextHandler& get_input();
} // namespace cs::input
