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

TEST_CASE("attemting to use keybinding after unbind")
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
}
