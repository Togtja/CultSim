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
};
namespace detail
{
/** Handles input for a certain context */
class InputHandler
{
private:
    robin_hood::unordered_map<SDL_Scancode, std::function<void()>> m_key_binding;
    KeyContext m_context_type;

public:
    /**
     * Handles input for a certain context
     *
     * @param type The Key Context that the input handler should handle
     */
    explicit InputHandler(KeyContext type);

    /**
     * Given a key and a function binds that key to that function in the input handlers context
     *
     * @param event The key event that you want to bind
     * @param function the function you want that key to be binded to
     */
    void bind_key(SDL_Scancode event, std::function<void()> function);

    /**
     * Unbinds a key in the input handlers context
     *
     * @param event The key even you want to unbind
     */
    void unbind_key(SDL_Scancode event);

    /**
     * Given a key, runs that key's binded function in the input handlers context
     *
     * @param event the key even you want to trigger
     */
    void handle_input(SDL_Scancode event);

    /**
     * Given an event, checks if this context has that event
     *
     * @param event The event you want to check
     *
     * @return true if the event is in the context else false
     */
    bool has_event(SDL_Scancode event);

    /**
     * Clears the key bindings
     */
    void clear();

    ~InputHandler();
};

} // namespace detail

class ContextHandler
{
private:
    std::vector<KeyContext> m_active_stack;
    robin_hood::unordered_map<KeyContext, detail::InputHandler> m_input_map;

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
    void add_context(KeyContext context);

    /**
     * Remove a context and the input handler
     *
     * @param context The context you want the remove the input handler for
     */
    void remove_context(KeyContext context);

    /**
     * Remove the top context from the context stack
     */
    void remove_context();

    /**
     * Bind context to a key, and that key to a function
     *
     * @param context The context you want to bind a key for
     * @param event The key you want to bind to the context
     * @param function The function you want you bind to the key event
     */
    void bind_key(KeyContext context, SDL_Scancode event, std::function<void()> function);

    /**
     * Unbind a key from a context
     *
     * @param context The context you want to unbind from
     * @param event The key you want to unbind from the context
     */
    void unbind_key(KeyContext context, SDL_Scancode event);

    /**
     * Handle's input from an event, goes through the context stack and runs the first found event matching function
     *
     * @param event The event you want to run
     */
    void handle_input(SDL_Scancode event);

    /**
     * Checks if the context have a mapping to an InputHandler
     *
     * @param KeyContext to check if exist in the mapping
     *
     * @return true if the context exist in the map, else false
     */
    bool has_context(KeyContext context);

    /**
     * Clears KeyContext stack, and the mappings
     */
    void clear();

private:
    ContextHandler();
};
/**
 * Get the input manager singleton
 *
 * @return The input manager singleton
 */
ContextHandler& get_input();
} // namespace cs::input
