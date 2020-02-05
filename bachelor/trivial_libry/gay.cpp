#include <doctest/doctest.h>
#include <spdlog/spdlog.h>

TEST_CASE("is_putin_gay")
{
    spdlog::info("Putin is actually gay!");
    REQUIRE(true);
}
