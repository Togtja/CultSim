#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cs::fs
{
/**
 * Initialize the virtual file system powered by PhysFS
 *
 * @warning Always call this function once before using any of the other ones in this namespace / file
 * @param project_name This argument should be argv[0] as per PhysFS documentation
 * @return true if successful, otherwise false
 */
bool init(std::string_view project_name);

/**
 * Deinitialize the virtual file system powered by PhysFS
 *
 * @warning Always call this function once before terminating the program, after having called init
 */
void deinit();

/**
 * Read the entire file at the relative file path
 *
 * @param rpath The relative file path to read from
 * @return A string containing the full contents of the file
 */
std::string read_file(std::string_view rpath);

/**
 * Read the entire file at the relative file path as bytes
 *
 * @param rpath The relative file path to read from
 * @return A vector of bytes read
 */
std::vector<uint8_t> read_byte_file(std::string_view rpath);

/**
 * Write the text stored in data to the given relative file path
 *
 * @note Write to 63 bit, where the 64th bit the error bit. If length is greater than 0x7FFFFFFFFFFFFFFF, this function will
 * immediately fail. It also creates necessary parent directories and the file if it does not exist
 * @param rpath The relative file path to read from
 * @param data The text data to write to the file
 * @return Number of bytes written, -1 if an error occurs
 */
uint64_t write_file(std::string_view rpath, const std::string& data);

/**
 * Checks if the file or directory at the given path exists
 *
 * @param rpath The relative file/directory path to read from
 * @return true if the file/directory exists, otherwise false
 */
bool exists(std::string_view rpath);

/**
 * Create a directory at the given path
 *
 * @param rpath The relative file path to read from
 * @return true if successful, otherwise false
 */
bool mkdir(std::string_view rpath);

/**
 * Move the file (and / or rename it it) from the old to the new path
 *
 * @param rpath_old The source file to rename / move
 * @param rpath_new The destination path / name
 * @return true if successful, otherwise false
 */
bool move_file(std::string_view rpath_old, std::string_view rpath_new);

/**
 * Delete a file or directory at the given path
 *
 * @note If directory it must be empty
 * @param rpath The relative file path to read from
 * @return true if successful, otherwise false
 */
bool delete_file(std::string_view rpath);

/**
 * Copy the file from the old to the new path
 *
 * @param rpath_old The source file to copy
 * @param rpath_new The destination path / name
 * @param overwrite_existing Set this to true if you want to overwrite existing files
 * @return true if successful, otherwise false
 */
bool copy_file(std::string_view rpath_old, std::string_view rpath_new, bool overwrite_existing = false);

/**
 * Get the latest error string from the underlying filesystem
 *
 * @return The latest errorstring
 */
std::string_view get_errorstring();

/**
 * Reads a directory, and returns a list of the files inside the directory
 *
 * @param rpath The relative path of the directory you want to list
 * @return List of all the files in the directory
 */
std::vector<std::string> list_directory(std::string_view rpath);

/**
 * Checks if the given path is a directory and exist
 *
 * @param rpath The relative path of the directory you want to check
 * @returns true if it is a directory otherwise false
 */
bool is_directory(std::string_view rpath);

/**
 * Checks if the given path is a file
 *
 * @param rpath The relative path of the file you want to check
 * @returns true if it is a file otherwise false
 */
bool is_file(std::string_view rpath);
} // namespace cs::fs
