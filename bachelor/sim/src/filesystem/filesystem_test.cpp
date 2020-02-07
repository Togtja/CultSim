#include "filesystem.h"

#include <string>

#include <doctest/doctest.h>

TEST_CASE("testing the file system")
{
    std::string name("test.txt");
    std::string data("This is a test");

    REQUIRE(cs::fs::init("cultsim_test") == true);
    CHECK(cs::fs::write_file(name, data) == data.length());
    CHECK(cs::fs::exists(name) == true);
    CHECK(cs::fs::read_file(name) == data);
    CHECK(cs::fs::delete_file(name) == true);
    CHECK(cs::fs::exists(name) == false);

    cs::fs::deinit();
}

TEST_CASE("fail to initialize twice")
{
    REQUIRE(cs::fs::init("app") == true);
    REQUIRE(cs::fs::init("app") == false);
    cs::fs::deinit();
}
