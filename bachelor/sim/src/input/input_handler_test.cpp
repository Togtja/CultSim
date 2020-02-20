#include "input_handler.h"

#include <doctest/doctest.h>

using namespace cs::input;
TEST_CASE("attempting to get input handler")
{
    REQUIRE(&get_input() != nullptr);
}

TEST_CASE("attempting to create a key and use it")
{
    auto& input  = get_input();
    bool success = false;
    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_A, [&success]() { success = true; });
    input.handle_input(SDL_SCANCODE_A);
    CHECK(success);
    input.clear();
}

TEST_CASE("attempting to use keybinding after clear")
{
    auto& input = get_input();
    int times   = 0;
    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F12, [&times]() { times++; });
    input.handle_input(SDL_SCANCODE_F12);
    input.handle_input(SDL_SCANCODE_F12);
    CHECK(times == 2);
    input.clear();
    input.handle_input(SDL_SCANCODE_F12);
    CHECK(times == 2);
}

TEST_CASE("attempting to use keybinding after unbind")
{
    auto& input = get_input();
    int times1 = 0, times0 = 0;
    // Binding 2 key to make sure that no other key also get unbinding
    // AKA somebody just "cleared" all
    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F11, [&times1]() { times1++; });
    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F10, [&times0]() { times0++; });
    input.handle_input(SDL_SCANCODE_F11);
    input.handle_input(SDL_SCANCODE_F10);
    CHECK(times1 == 1);
    CHECK(times0 == 1);
    input.unbind_key(KeyContext::DefaultContext, SDL_SCANCODE_F10);
    input.handle_input(SDL_SCANCODE_F11);
    input.handle_input(SDL_SCANCODE_F10);
    CHECK(times1 == 2);
    CHECK(times0 == 1);
    input.clear();
}

TEST_CASE("attempting to bind same key to different function without overwrite")
{
    auto& input = get_input();
    int times1 = 0, times0 = 0;

    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19, [&times1]() { times1++; });
    input.handle_input(SDL_SCANCODE_F19);
    CHECK(times1 == 1);

    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19, [&times0]() { times0++; });
    input.handle_input(SDL_SCANCODE_F19);
    CHECK(times1 == 2);
    CHECK(times0 == 0);
    input.clear();
}
TEST_CASE("attempting to bind same key to different function with overwrite")
{
    auto& input = get_input();
    int times1 = 0, times0 = 0;

    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19, [&times1]() { times1++; });
    input.handle_input(SDL_SCANCODE_F19);
    CHECK(times1 == 1);

    input.bind_key(
        KeyContext::DefaultContext,
        SDL_SCANCODE_F19,
        [&times0]() { times0++; },
        true);
    input.handle_input(SDL_SCANCODE_F19);
    CHECK(times1 == 1);
    CHECK(times0 == 1);
    input.clear();
}

TEST_CASE("attempting to unbind something that is already unbound")
{
    auto& input = get_input();
    int times   = 0;
    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19, [&times]() { times++; });
    input.handle_input(SDL_SCANCODE_F19);
    CHECK(times == 1);

    input.unbind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19);
    input.handle_input(SDL_SCANCODE_F19);
    CHECK(times == 1);

    input.unbind_key(KeyContext::DefaultContext, SDL_SCANCODE_F19);
    input.handle_input(SDL_SCANCODE_F19);
    CHECK(times == 1);
    input.clear();
}

TEST_CASE("attempting to add default context on top of something else")
{
    auto& input = get_input();
    int times1 = 0, times0 = 0;

    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F13, [&times1]() { times1++; });
    input.bind_key(KeyContext::Agent, SDL_SCANCODE_F13, [&times0]() { times0++; });
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 1);
    CHECK(times0 == 0);
    input.add_context(KeyContext::Agent);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 1);
    CHECK(times0 == 1);
    // Adding default is not allowed, and it is always at the bottom of the context stack
    input.add_context(KeyContext::DefaultContext);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 1);
    CHECK(times0 == 2);
    input.clear();
}

TEST_CASE("attempting to add non default context on top of another one")
{
    auto& input = get_input();
    int times1 = 0, times0 = 0;

    input.bind_key(KeyContext::Agent, SDL_SCANCODE_F13, [&times1]() { times1++; });
    input.bind_key(KeyContext::AgentOnHover, SDL_SCANCODE_F13, [&times0]() { times0++; });
    // Nothing should happend as the context are not on the stack
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 0);
    CHECK(times0 == 0);
    input.add_context(KeyContext::Agent);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 1);
    CHECK(times0 == 0);
    // Adding AgentOnHover, on top
    input.add_context(KeyContext::AgentOnHover);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 1);
    CHECK(times0 == 1);
    // Removed old Agent that was under AgentOnHover, and put it above again
    input.add_context(KeyContext::Agent);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 2);
    CHECK(times0 == 1);
    input.clear();
}

TEST_CASE("attempting to remove default context from stack")
{
    auto& input = get_input();
    int times   = 0;

    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F24, [&times]() { times++; });
    input.handle_input(SDL_SCANCODE_F24);
    CHECK(times == 1);

    input.remove_context(KeyContext::DefaultContext);
    input.handle_input(SDL_SCANCODE_F24);
    CHECK(times == 2);
    input.clear();
}

TEST_CASE("attempting to remove non default context from stack by context")
{
    auto& input = get_input();
    int times1 = 0, times0 = 0;

    input.bind_key(KeyContext::Agent, SDL_SCANCODE_F11, [&times1]() { times1++; });
    input.bind_key(KeyContext::AgentOnHover, SDL_SCANCODE_F13, [&times0]() { times0++; });
    // Nothing should happend as the context are not on the stack
    input.handle_input(SDL_SCANCODE_F11);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 0);
    CHECK(times0 == 0);
    input.add_context(KeyContext::AgentOnHover);
    input.add_context(KeyContext::Agent);
    input.handle_input(SDL_SCANCODE_F11);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 1);
    CHECK(times0 == 1);

    // Removed AgentOnHover
    input.remove_context(KeyContext::AgentOnHover);
    input.handle_input(SDL_SCANCODE_F11);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 2);
    CHECK(times0 == 1);

    input.add_context(KeyContext::AgentOnHover);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times0 == 2);
    input.clear();
}

TEST_CASE("attempting to remove top non default context from stack")
{
    auto& input = get_input();
    int times1 = 0, times0 = 0;
    input.bind_key(KeyContext::Agent, SDL_SCANCODE_F11, [&times1]() { times1++; });
    input.bind_key(KeyContext::AgentOnHover, SDL_SCANCODE_F13, [&times0]() { times0++; });
    // Nothing should happend as the context are not on the stack
    input.handle_input(SDL_SCANCODE_F11);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 0);
    CHECK(times0 == 0);
    input.add_context(KeyContext::AgentOnHover);
    input.add_context(KeyContext::Agent);
    input.handle_input(SDL_SCANCODE_F11);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 1);
    CHECK(times0 == 1);

    // Removed top (which is Agent)
    input.remove_context();
    input.handle_input(SDL_SCANCODE_F11);
    input.handle_input(SDL_SCANCODE_F13);
    CHECK(times1 == 1);
    CHECK(times0 == 2);
    // Add it again to make sure it is not deleted
    input.add_context(KeyContext::Agent);
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times1 == 2);
    input.clear();
}

TEST_CASE("attempting to clear context from all bindings")
{
    auto& input = get_input();
    int times   = 0;

    input.bind_key(KeyContext::Agent, SDL_SCANCODE_F11, [&times]() { times++; });
    // Nothing should happend as the context are not on the stack
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times == 0);
    input.add_context(KeyContext::Agent);
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times == 1);

    // Removed AgentOnHover
    input.clear_context(KeyContext::Agent);
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times == 1);
    // Note how we don't need to add it to the context again
    input.bind_key(KeyContext::Agent, SDL_SCANCODE_F11, [&times]() { times++; });
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times == 2);
    input.clear();
}

TEST_CASE("attempting to go back to default context")
{
    auto& input = get_input();
    int times0 = 0, times1 = 0, times2 = 0;

    input.bind_key(KeyContext::DefaultContext, SDL_SCANCODE_F11, [&times0]() { times0++; });
    input.bind_key(KeyContext::Agent, SDL_SCANCODE_F11, [&times1]() { times1++; });
    input.bind_key(KeyContext::AgentOnHover, SDL_SCANCODE_F11, [&times2]() { times2++; });
    // Only deafult is on the context stack
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times0 == 1);
    CHECK(times1 == 0);
    CHECK(times2 == 0);

    input.add_context(KeyContext::Agent);
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times0 == 1);
    CHECK(times1 == 1);
    CHECK(times2 == 0);

    input.add_context(KeyContext::AgentOnHover);
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times0 == 1);
    CHECK(times1 == 1);
    CHECK(times2 == 1);

    // Going back to default
    input.back_to_default();
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times0 == 2);
    CHECK(times1 == 1);
    CHECK(times2 == 1);

    input.add_context(KeyContext::Agent);
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times0 == 2);
    CHECK(times1 == 2);
    CHECK(times2 == 1);

    input.add_context(KeyContext::AgentOnHover);
    input.handle_input(SDL_SCANCODE_F11);
    CHECK(times0 == 2);
    CHECK(times1 == 2);
    CHECK(times2 == 2);

    input.clear();
}