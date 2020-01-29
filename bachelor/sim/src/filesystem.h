#pragma once

#include <string>
#include <string_view>

namespace cs
{
namespace fs
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
 * Write the text stored in data to the given relative file path
 *
 * @note Also creates necessary parent directories and the file if it does not exist
 * @param rpath The relative file path to read from
 * @param data The text data to write to the file
 * @return Number of bytes written, -1 if an error occurs
 */
int64_t write_file(std::string_view rpath, const std::string& data);

/**
 * Checks if the file at the given path exists
 *
 * @param rpath The relative file path to read from
 * @return true if the file exists, otherwise false
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
 * @note Also deletes all sub-directories files if any
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

} // namespace fs
} // namespace cs
