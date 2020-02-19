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

TEST_CASE("attempting to initialize twice")
{
    REQUIRE(cs::fs::init("app"));
    REQUIRE(!cs::fs::init("app"));
    cs::fs::deinit();
}

TEST_CASE("attempting to find path non-existing file")
{
    std::string file("fake.txt");

    REQUIRE(cs::fs::init("cultsim_test"));
    CHECK(!cs::fs::exists(file));
    CHECK(cs::fs::read_file(file) == "");
    CHECK(!cs::fs::delete_file(file));

    cs::fs::deinit();
}

TEST_CASE("attempting directory creation and deletion")
{
    std::string dir("directory");
    std::string file_in_dir(dir + "/test.txt");

    REQUIRE(cs::fs::init("cultsim_test"));
    // Checking that it does not exist
    CHECK(!cs::fs::is_directory(dir));
    CHECK(cs::fs::list_directory(dir).size() == 0);
    // Making the directory
    CHECK(cs::fs::mkdir(dir));
    CHECK(cs::fs::is_directory(dir));
    // Should still be empty
    CHECK(cs::fs::list_directory(dir).size() == 0);
    // Creating an empty file
    CHECK(cs::fs::write_file(file_in_dir, "") == 0);
    CHECK(cs::fs::exists(file_in_dir));
    CHECK(cs::fs::list_directory(dir).size() == 1);

    // Directory not empty so delete fails
    CHECK(!cs::fs::delete_file(dir));
    CHECK(cs::fs::is_directory(dir));
    // Emptying the directory
    CHECK(cs::fs::delete_file(file_in_dir));
    CHECK(cs::fs::delete_file(dir));
    // Making sure it is gone
    CHECK(!cs::fs::is_directory(dir));
    CHECK(!cs::fs::exists(file_in_dir));
    CHECK(cs::fs::list_directory(dir).size() == 0);

    cs::fs::deinit();
}

TEST_CASE("attempting to read_file of directory")
{
    std::string dir("directory2");

    REQUIRE(cs::fs::init("cultsim_test"));
    // Making sure it is not there
    CHECK(!cs::fs::is_directory(dir));
    // Creating it
    CHECK(cs::fs::mkdir(dir));
    CHECK(cs::fs::is_directory(dir));
    // Trying to read it
    CHECK(cs::fs::read_file(dir) == "");
    // Delete it
    CHECK(cs::fs::delete_file(dir));
    cs::fs::deinit();
}

TEST_CASE("attempting to write_file on a directory")
{
    std::string dir("directory3");

    REQUIRE(cs::fs::init("cultsim_test"));
    // Making sure it is not there
    CHECK(!cs::fs::is_directory(dir));
    // Creating it
    CHECK(cs::fs::mkdir(dir));
    CHECK(cs::fs::is_directory(dir));
    // Note the 64th bit is assigned as error from PhysFS
    CHECK(cs::fs::write_file(dir, "this should fail") == static_cast<uint64_t>(-1));
    CHECK(cs::fs::delete_file(dir));

    cs::fs::deinit();
}

TEST_CASE("attempting copying file")
{
    std::string name_og("original.txt");
    std::string data("I will be copied");
    std::string name_cpy("copy.txt");

    REQUIRE(cs::fs::init("cultsim_test"));

    CHECK(!cs::fs::exists(name_og));
    CHECK(!cs::fs::exists(name_cpy));
    // Creating the original
    CHECK(cs::fs::write_file(name_og, data) == data.length());
    CHECK(cs::fs::exists(name_og));
    CHECK(!cs::fs::exists(name_cpy));
    // Copying it
    CHECK(cs::fs::copy_file(name_og, name_cpy));
    CHECK(cs::fs::exists(name_og));
    CHECK(cs::fs::exists(name_cpy));
    // Making sure they are equal
    CHECK(cs::fs::read_file(name_og) == cs::fs::read_file(name_cpy));
    // Delete the original
    CHECK(cs::fs::delete_file(name_og));
    CHECK(!cs::fs::exists(name_og));
    CHECK(cs::fs::exists(name_cpy));
    // Delete the copy
    CHECK(cs::fs::delete_file(name_cpy));
    CHECK(!cs::fs::exists(name_og));
    CHECK(!cs::fs::exists(name_cpy));

    cs::fs::deinit();
}

TEST_CASE("attempting to copy to same file")
{
    std::string name("same_place.txt");
    std::string data("I will try to be copied to same spot");

    REQUIRE(cs::fs::init("cultsim_test"));
    CHECK(!cs::fs::exists(name));
    // Creating name
    CHECK(cs::fs::write_file(name, data) == data.length());
    CHECK(cs::fs::exists(name));
    // Trying to copy to the same place
    CHECK(!cs::fs::copy_file(name, name));

    CHECK(cs::fs::exists(name));
    CHECK(cs::fs::delete_file(name));
    CHECK(!cs::fs::exists(name));

    cs::fs::deinit();
}
