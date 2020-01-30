#include <functional>
#include <map>
#include <vector>

#include <SDL_events.h>
namespace cs
{
namespace input
{
enum class KeyContext
{
    DefaultContext,
    Agent,
    AgentOnHover,
};

class InputHandler
{
private:
    std::map<SDL_Scancode, std::function<void()>> m_key_binding;
    KeyContext m_context_type;

public:
    explicit InputHandler(KeyContext type);
    void bind_key(const SDL_Scancode event, const std::function<void()> function);
    void unbind_key(const SDL_Scancode event);
    void handle_input(const SDL_Scancode event);
    ~InputHandler();
};
} // namespace input
} // namespace cs
