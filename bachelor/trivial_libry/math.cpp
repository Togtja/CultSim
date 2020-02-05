#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

TEST_CASE("add integers")
{
    REQUIRE(2 + 2 == 4);
    REQUIRE(2 + -2 + 3 - 1 + 4 - 3 - 2 == 1);
}

TEST_CASE("multiply floats")
{
    REQUIRE(2.f * 0.5f == 1);
}
