#include "input_handler.h"

#include <doctest/doctest.h>

using namespace cs::input;
TEST_CASE("attempting to get input handler")
{
    REQUIRE(&get_input() != nullptr);
}

TEST_CASE("attempting to create a key and use it")
{
    SDL_Event e{};
    e.type                = SDL_KEYDOWN;
    e.key.keysym.scancode = SDL_SCANCODE_A;

    auto& input  = get_input();
    bool success = false;
    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_A, Action::MoveLeft, [&success]() { success = true; });
    input.handle_input(e);
    CHECK(success);
    input.clear();
}
TEST_CASE("attempting to use keybinding after clear")
{
    SDL_Event e{};
    e.type                = SDL_KEYDOWN;
    e.key.keysym.scancode = SDL_SCANCODE_F12;

    auto& input = get_input();
    int times   = 0;
    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F12, Action::ZoomOut, [&times]() { times++; });
    input.handle_input(e);
    input.handle_input(e);
    CHECK(times == 2);
    input.clear();
    input.handle_input(e);
    CHECK(times == 2);
}

TEST_CASE("attempting to use keybinding after unbind")
{
    SDL_Event f11{};
    f11.type                = SDL_KEYDOWN;
    f11.key.keysym.scancode = SDL_SCANCODE_F11;

    SDL_Event f10{};
    f10.type                = SDL_KEYDOWN;
    f10.key.keysym.scancode = SDL_SCANCODE_F10;

    auto& input = get_input();
    int times1 = 0, times0 = 0;
    // Binding 2 key to make sure that no other key also get unbinding
    // AKA somebody just "cleared" all
    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F11, Action::MoveLeft, [&times1]() { times1++; });
    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F10, Action::ZoomIn, [&times0]() { times0++; });
    input.handle_input(f11);
    input.handle_input(f10);
    CHECK(times1 == 1);
    CHECK(times0 == 1);
    input.unbind_key(KeyContext::DefaultContext, SDL_SCANCODE_F10);
    input.handle_input(f11);
    input.handle_input(f10);
    CHECK(times1 == 2);
    CHECK(times0 == 1);
    input.clear();
}

TEST_CASE("attempting to bind same key to different function")
{
    SDL_Event e{};
    e.type                = SDL_KEYDOWN;
    e.key.keysym.scancode = SDL_SCANCODE_F19;

    auto& input = get_input();
    int times1 = 0, times0 = 0;

    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19, Action::MoveLeft, [&times1]() { times1++; });
    input.handle_input(e);
    CHECK(times1 == 1);

    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19, Action::MoveLeft, [&times0]() { times0++; });
    input.handle_input(e);
    CHECK(times1 == 1);
    CHECK(times0 == 1);
    input.clear();
}

TEST_CASE("attempting to bind diffrent key to same action")
{
    SDL_Event f19{};
    f19.type                = SDL_KEYDOWN;
    f19.key.keysym.scancode = SDL_SCANCODE_F19;
    SDL_Event f18{};
    f18.type                = SDL_KEYDOWN;
    f18.key.keysym.scancode = SDL_SCANCODE_F18;

    auto& input = get_input();
    int times   = 0;

    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19, Action::MoveLeft, [&times]() { times++; });
    input.handle_input(f19);
    CHECK(times == 1);

    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F18, Action::MoveLeft);
    input.handle_input(f18);
    CHECK(times == 2);
    input.clear();
}

TEST_CASE("attempting to unbind something that is already unbound")
{
    SDL_Event e{};
    e.type                = SDL_KEYDOWN;
    e.key.keysym.scancode = SDL_SCANCODE_F19;

    auto& input = get_input();
    int times   = 0;
    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19, Action::MoveLeft, [&times]() { times++; });
    input.handle_input(e);
    CHECK(times == 1);

    input.unbind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19);
    input.handle_input(e);
    CHECK(times == 1);

    input.unbind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19);
    input.handle_input(e);
    CHECK(times == 1);
    input.clear();
}

TEST_CASE("attempting to add default context on top of something else")
{
    SDL_Event e{};
    e.type                = SDL_KEYDOWN;
    e.key.keysym.scancode = SDL_SCANCODE_F13;

    auto& input = get_input();
    int times1 = 0, times0 = 0;

    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F13, Action::MoveLeft, [&times1]() { times1++; });
    input.fast_bind_key(KeyContext::Agent, SDL_SCANCODE_F13, Action::MoveLeft, [&times0]() { times0++; });
    input.handle_input(e);
    CHECK(times1 == 1);
    CHECK(times0 == 0);
    input.add_context(KeyContext::Agent);
    input.handle_input(e);
    CHECK(times1 == 1);
    CHECK(times0 == 1);
    // Adding default is not allowed, and it is always at the bottom of the context stack
    input.add_context(KeyContext::DefaultContext);
    input.handle_input(e);
    CHECK(times1 == 1);
    CHECK(times0 == 2);
    input.clear();
}

TEST_CASE("attempting to add non default context on top of another one")
{
    SDL_Event e{};
    e.type                = SDL_KEYDOWN;
    e.key.keysym.scancode = SDL_SCANCODE_F13;

    auto& input = get_input();
    int times1 = 0, times0 = 0;

    input.fast_bind_key(KeyContext::Agent, SDL_SCANCODE_F13, Action::MoveLeft, [&times1]() { times1++; });
    input.fast_bind_key(KeyContext::AgentOnHover, SDL_SCANCODE_F13, Action::MoveLeft, [&times0]() { times0++; });
    // Nothing should happend as the context are not on the stack
    input.handle_input(e);
    CHECK(times1 == 0);
    CHECK(times0 == 0);
    input.add_context(KeyContext::Agent);
    input.handle_input(e);
    CHECK(times1 == 1);
    CHECK(times0 == 0);
    // Adding AgentOnHover, on top
    input.add_context(KeyContext::AgentOnHover);
    input.handle_input(e);
    CHECK(times1 == 1);
    CHECK(times0 == 1);
    // Removed old Agent that was under AgentOnHover, and put it above again
    input.add_context(KeyContext::Agent);
    input.handle_input(e);
    CHECK(times1 == 2);
    CHECK(times0 == 1);
    input.clear();
}

TEST_CASE("attempting to remove default context from stack")
{
    SDL_Event e{};
    e.type                = SDL_KEYDOWN;
    e.key.keysym.scancode = SDL_SCANCODE_F24;

    auto& input = get_input();
    int times   = 0;

    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F24, Action::MoveLeft, [&times]() { times++; });
    input.handle_input(e);
    CHECK(times == 1);

    input.remove_context(KeyContext::DefaultContext);
    input.handle_input(e);
    CHECK(times == 2);
    input.clear();
}

TEST_CASE("attempting to pop default context from stack")
{
    SDL_Event e{};
    e.type                = SDL_KEYDOWN;
    e.key.keysym.scancode = SDL_SCANCODE_F24;
    auto& input           = get_input();
    int times             = 0;

    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F24, Action::MoveLeft, [&times]() { times++; });
    input.handle_input(e);
    CHECK(times == 1);

    input.pop_context();
    input.handle_input(e);
    CHECK(times == 2);
    input.clear();
}

TEST_CASE("attempting to remove non default context from stack by context")
{
    SDL_Event f11{};
    f11.type                = SDL_KEYDOWN;
    f11.key.keysym.scancode = SDL_SCANCODE_F11;

    SDL_Event f13{};
    f13.type                = SDL_KEYDOWN;
    f13.key.keysym.scancode = SDL_SCANCODE_F13;

    auto& input = get_input();
    int times1 = 0, times0 = 0;

    input.fast_bind_key(KeyContext::Agent, SDL_SCANCODE_F11, Action::MoveLeft, [&times1]() { times1++; });
    input.fast_bind_key(KeyContext::AgentOnHover, SDL_SCANCODE_F13, Action::MoveLeft, [&times0]() { times0++; });
    // Nothing should happend as the context are not on the stack
    input.handle_input(f11);
    input.handle_input(f13);
    CHECK(times1 == 0);
    CHECK(times0 == 0);
    input.add_context(KeyContext::AgentOnHover);
    input.add_context(KeyContext::Agent);
    input.handle_input(f11);
    input.handle_input(f13);
    CHECK(times1 == 1);
    CHECK(times0 == 1);

    // Removed AgentOnHover
    input.remove_context(KeyContext::AgentOnHover);
    input.handle_input(f11);
    input.handle_input(f13);
    CHECK(times1 == 2);
    CHECK(times0 == 1);

    input.add_context(KeyContext::AgentOnHover);
    input.handle_input(f13);
    CHECK(times0 == 2);
    input.clear();
}

TEST_CASE("attempting to remove top non default context from stack")
{
    SDL_Event f11{};
    f11.type                = SDL_KEYDOWN;
    f11.key.keysym.scancode = SDL_SCANCODE_F11;

    SDL_Event f13{};
    f13.type                = SDL_KEYDOWN;
    f13.key.keysym.scancode = SDL_SCANCODE_F13;

    auto& input = get_input();
    int times1 = 0, times0 = 0;
    input.fast_bind_key(KeyContext::Agent, SDL_SCANCODE_F11, Action::MoveLeft, [&times1]() { times1++; });
    input.fast_bind_key(KeyContext::AgentOnHover, SDL_SCANCODE_F13, Action::MoveLeft, [&times0]() { times0++; });
    // Nothing should happend as the context are not on the stack
    input.handle_input(f11);
    input.handle_input(f13);
    CHECK(times1 == 0);
    CHECK(times0 == 0);
    input.add_context(KeyContext::AgentOnHover);
    input.add_context(KeyContext::Agent);
    input.handle_input(f11);
    input.handle_input(f13);
    CHECK(times1 == 1);
    CHECK(times0 == 1);

    // Removed top (which is Agent)
    input.pop_context();
    input.handle_input(f11);
    input.handle_input(f13);
    CHECK(times1 == 1);
    CHECK(times0 == 2);
    // Add it again to make sure it is not deleted
    input.add_context(KeyContext::Agent);
    input.handle_input(f11);
    CHECK(times1 == 2);
    input.clear();
}

TEST_CASE("attempting to clear context from all bindings")
{
    SDL_Event f11{};
    f11.type                = SDL_KEYDOWN;
    f11.key.keysym.scancode = SDL_SCANCODE_F11;

    auto& input = get_input();
    int times   = 0;

    input.fast_bind_key(KeyContext::Agent, SDL_SCANCODE_F11, Action::MoveLeft, [&times]() { times++; });
    // Nothing should happend as the context are not on the stack
    input.handle_input(f11);
    CHECK(times == 0);
    input.add_context(KeyContext::Agent);
    input.handle_input(f11);
    CHECK(times == 1);

    // Removed AgentOnHover
    input.clear_context(KeyContext::Agent);
    input.handle_input(f11);
    CHECK(times == 1);
    // Note how we don't need to add it to the context again
    input.fast_bind_key(KeyContext::Agent, SDL_SCANCODE_F11, Action::MoveLeft, [&times]() { times++; });
    input.handle_input(f11);
    CHECK(times == 2);
    input.clear();
}

TEST_CASE("attempting to go back to default context")
{
    SDL_Event e{};
    e.type                = SDL_KEYDOWN;
    e.key.keysym.scancode = SDL_SCANCODE_F11;

    auto& input = get_input();
    int times0 = 0, times1 = 0, times2 = 0;

    input.fast_bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F11, Action::MoveLeft, [&times0]() { times0++; });
    input.fast_bind_key(KeyContext::Agent, SDL_SCANCODE_F11, Action::MoveLeft, [&times1]() { times1++; });
    input.fast_bind_key(KeyContext::AgentOnHover, SDL_SCANCODE_F11, Action::MoveLeft, [&times2]() { times2++; });
    // Only deafult is on the context stack
    input.handle_input(e);
    CHECK(times0 == 1);
    CHECK(times1 == 0);
    CHECK(times2 == 0);

    input.add_context(KeyContext::Agent);
    input.handle_input(e);
    CHECK(times0 == 1);
    CHECK(times1 == 1);
    CHECK(times2 == 0);

    input.add_context(KeyContext::AgentOnHover);
    input.handle_input(e);
    CHECK(times0 == 1);
    CHECK(times1 == 1);
    CHECK(times2 == 1);

    // Going back to default
    input.back_to_default();
    input.handle_input(e);
    CHECK(times0 == 2);
    CHECK(times1 == 1);
    CHECK(times2 == 1);

    input.add_context(KeyContext::Agent);
    input.handle_input(e);
    CHECK(times0 == 2);
    CHECK(times1 == 2);
    CHECK(times2 == 1);

    input.add_context(KeyContext::AgentOnHover);
    input.handle_input(e);
    CHECK(times0 == 2);
    CHECK(times1 == 2);
    CHECK(times2 == 2);

    input.clear();
}
