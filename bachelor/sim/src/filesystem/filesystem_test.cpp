#include "filesystem.h"

#include <string>

#include <doctest/doctest.h>

TEST_CASE("testing the file system")
{
    std::string name("test.txt");
    std::string data("This is a test");

    REQUIRE(cs::fs::init("cultsim_test"));
    CHECK(cs::fs::write_file(name, data) == data.length());
    CHECK(cs::fs::exists(name));
    CHECK(cs::fs::read_file(name) == data);
    CHECK(cs::fs::delete_file(name));
    CHECK(!cs::fs::exists(name));

    cs::fs::deinit();
}

TEST_CASE("fail to initialize twice")
{
    REQUIRE(cs::fs::init("app"));
    REQUIRE(!cs::fs::init("app"));
    cs::fs::deinit();
}
