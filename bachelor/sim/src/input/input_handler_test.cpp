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
}

