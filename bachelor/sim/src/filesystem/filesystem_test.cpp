#include "filesystem.h"

#include <string>

#include <doctest/doctest.h>

TEST_CASE("testing the file system")
{
    std::string name("test.txt");
    std::string data("This is a test");

    REQUIRE(cs::fs::init("cultsim_test"));
    REQUIRE(!cs::fs::exists(name));
    CHECK(cs::fs::write_file(name, data) == data.length());
    CHECK(cs::fs::exists(name));
    CHECK(cs::fs::read_file(name) == data);
    CHECK(cs::fs::delete_file(name));
    CHECK(!cs::fs::exists(name));

    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting to initialize twice")
{
    REQUIRE(cs::fs::init("app"));
    REQUIRE(!cs::fs::init("app"));
    REQUIRE(cs::fs::deinit());
}
TEST_CASE("attempting to deinitialize twice")
{
    REQUIRE(cs::fs::init("app"));
    REQUIRE(cs::fs::deinit());
    REQUIRE(!cs::fs::deinit());
}

TEST_CASE("attempting to find path non-existing file")
{
    std::string file("fake.txt");

    REQUIRE(cs::fs::init("cultsim_test"));
    REQUIRE(!cs::fs::exists(file));
    CHECK(cs::fs::read_file(file) == "");
    CHECK(!cs::fs::delete_file(file));

    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting directory creation and deletion")
{
    std::string dir("directory");
    std::string file_in_dir(dir + "/test.txt");

    REQUIRE(cs::fs::init("cultsim_test"));
    // Checking that it does not exist
    REQUIRE(!cs::fs::is_directory(dir));
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

    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting to read_file of directory")
{
    std::string dir("directory2");

    REQUIRE(cs::fs::init("cultsim_test"));
    // Making sure it is not there
    REQUIRE(!cs::fs::is_directory(dir));
    // Creating it
    CHECK(cs::fs::mkdir(dir));
    CHECK(cs::fs::is_directory(dir));
    // Trying to read it
    CHECK(cs::fs::read_file(dir) == "");
    // Delete it
    CHECK(cs::fs::delete_file(dir));
    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting to write_file on a directory")
{
    std::string dir("directory3");

    REQUIRE(cs::fs::init("cultsim_test"));
    // Making sure it is not there
    REQUIRE(!cs::fs::is_directory(dir));
    // Creating it
    CHECK(cs::fs::mkdir(dir));
    CHECK(cs::fs::is_directory(dir));
    // Note the 64th bit is assigned as error from PhysFS
    CHECK(cs::fs::write_file(dir, "this should fail") == static_cast<uint64_t>(-1));
    CHECK(cs::fs::delete_file(dir));

    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting copying file")
{
    std::string name_og("original.txt");
    std::string data("I will be copied");
    std::string name_cpy("copy.txt");

    REQUIRE(cs::fs::init("cultsim_test"));

    REQUIRE(!cs::fs::exists(name_og));
    REQUIRE(!cs::fs::exists(name_cpy));
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

    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting to copy to same file")
{
    std::string name("same_place.txt");
    std::string data("I will try to be copied to same spot");

    REQUIRE(cs::fs::init("cultsim_test"));
    REQUIRE(!cs::fs::exists(name));
    // Creating name
    CHECK(cs::fs::write_file(name, data) == data.length());
    CHECK(cs::fs::exists(name));
    // Trying to copy to the same place
    CHECK(!cs::fs::copy_file(name, name));

    CHECK(cs::fs::exists(name));
    CHECK(cs::fs::delete_file(name));
    CHECK(!cs::fs::exists(name));

    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting to copy to overwrite without sufficient permissions")
{
    std::string name("overwrite.txt");
    std::string data("I will try to overwrite target");
    std::string target("target.txt");
    std::string t_data("I am an innocent file");

    REQUIRE(cs::fs::init("cultsim_test"));
    REQUIRE(!cs::fs::exists(name));
    REQUIRE(!cs::fs::exists(target));
    // Creating the name (overwriting file)
    CHECK(cs::fs::write_file(name, data) == data.length());
    CHECK(cs::fs::exists(name));
    CHECK(!cs::fs::exists(target));
    // Creating the target file
    CHECK(cs::fs::write_file(target, t_data) == t_data.length());
    CHECK(cs::fs::exists(name));
    CHECK(cs::fs::exists(target));

    // Trying to overwrite the target file
    CHECK(!cs::fs::copy_file(name, target));

    // Making sure nothing got overwritten
    CHECK(cs::fs::exists(name));
    CHECK(cs::fs::exists(target));
    CHECK(cs::fs::read_file(name) != cs::fs::read_file(target));
    // Clean up after us
    CHECK(cs::fs::delete_file(name));
    CHECK(cs::fs::delete_file(target));
    CHECK(!cs::fs::exists(name));
    CHECK(!cs::fs::exists(target));

    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting to copy to overwrite WITH sufficient permissions")
{
    std::string name("overwrite2.txt");
    std::string data("I will try to overwrite target");
    std::string target("target2.txt");
    std::string t_data("I am an innocent file");

    REQUIRE(cs::fs::init("cultsim_test"));
    REQUIRE(!cs::fs::exists(name));
    REQUIRE(!cs::fs::exists(target));
    // Creating the name (overwriting file)
    CHECK(cs::fs::write_file(name, data) == data.length());
    CHECK(cs::fs::exists(name));
    CHECK(!cs::fs::exists(target));
    // Creating the target file
    CHECK(cs::fs::write_file(target, t_data) == t_data.length());
    CHECK(cs::fs::exists(name));
    CHECK(cs::fs::exists(target));
    CHECK(cs::fs::read_file(name) != cs::fs::read_file(target));

    // Trying to overwrite the target file
    CHECK(cs::fs::copy_file(name, target, true));

    // Making sure we overwrote
    CHECK(cs::fs::exists(name));
    CHECK(cs::fs::exists(target));
    CHECK(cs::fs::read_file(name) == cs::fs::read_file(target));
    // Clean up after us
    CHECK(cs::fs::delete_file(name));
    CHECK(cs::fs::delete_file(target));
    CHECK(!cs::fs::exists(name));
    CHECK(!cs::fs::exists(target));

    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting to use copy_file to copy directory")
{
    std::string dir("og_dir");
    std::string fake_dir("fake_dir");
    REQUIRE(cs::fs::init("cultsim_test"));

    REQUIRE(!cs::fs::exists(dir));
    REQUIRE(!cs::fs::exists(fake_dir));

    // Create the directory
    CHECK(cs::fs::mkdir(dir));
    CHECK(cs::fs::exists(dir));
    // Attempting to copy the directory
    CHECK(!cs::fs::copy_file(dir, "should_fail"));
    CHECK(cs::fs::exists(dir));
    CHECK(!cs::fs::exists(fake_dir));

    // Clean up
    CHECK(cs::fs::delete_file(dir));
    CHECK(!cs::fs::exists(dir));

    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting to move file")
{
    std::string from("from.txt");
    std::string data("I will be moved");
    std::string to("to.txt");

    REQUIRE(cs::fs::init("cultsim_test"));
    REQUIRE(!cs::fs::exists(from));
    REQUIRE(!cs::fs::exists(to));
    // Creating the original from
    CHECK(cs::fs::write_file(from, data) == data.length());
    CHECK(cs::fs::read_file(from) == data); // Making sure it has the correct data
    CHECK(cs::fs::exists(from));
    CHECK(!cs::fs::exists(to));
    // Moving the file
    CHECK(cs::fs::move_file(from, to));
    CHECK(!cs::fs::exists(from));
    CHECK(cs::fs::exists(to));
    // Checking the new file data
    CHECK(cs::fs::read_file(to) == data);
    // Clean up
    CHECK(cs::fs::delete_file(to));
    CHECK(!cs::fs::exists(to));

    REQUIRE(cs::fs::deinit());
}

TEST_CASE("attempting to use move_file as overwrite")
{
    std::string name("overwrite3.txt");
    std::string data("I will try to overwrite target");
    std::string target("target3.txt");
    std::string t_data("I am an innocent file");
    REQUIRE(cs::fs::init("cultsim_test"));
    CHECK(!cs::fs::exists(name));
    CHECK(!cs::fs::exists(target));
    // Creating the name (overwriting file)
    CHECK(cs::fs::write_file(name, data) == data.length());
    CHECK(cs::fs::exists(name));
    CHECK(!cs::fs::exists(target));
    // Creating the target file
    CHECK(cs::fs::write_file(target, t_data) == t_data.length());
    CHECK(cs::fs::exists(name));
    CHECK(cs::fs::exists(target));

    // Trying to overwrite the target file
    CHECK(!cs::fs::move_file(name, target));

    // Making sure nothing got overwritten
    CHECK(cs::fs::exists(name));
    CHECK(cs::fs::exists(target));
    CHECK(cs::fs::read_file(name) != cs::fs::read_file(target));
    // Clean up after us
    CHECK(cs::fs::delete_file(name));
    CHECK(cs::fs::delete_file(target));
    CHECK(!cs::fs::exists(name));
    CHECK(!cs::fs::exists(target));

    REQUIRE(cs::fs::deinit());
}