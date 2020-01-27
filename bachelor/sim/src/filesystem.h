#pragma once

#include <string>
#include <string_view>

namespace cs
{
namespace fs
{
/**
 *
 *
 *
 */
bool init(std::string_view project_name);

/**
 *
 *
 *
 */
void deinit();

/**
 *
 *
 *
 */
std::string read_file(std::string_view rpath);

/**
 *
 *
 *
 */
int64_t write_file(std::string_view rpath, const std::string& data);

/**
 *
 *
 *
 */
bool exists(std::string_view rpath);

/**
 *
 *
 *
 */
bool mkdir(std::string_view rpath);

/**
 *
 *
 *
 */
bool mkfile(std::string_view rpath);

/**
 *
 *
 *
 */
bool rename_file(std::string_view rpath_old, std::string_view rpath_new);

/**
 * @note Can delete directories
 */
bool delete_file(std::string_view rpath);

} // namespace fs
} // namespace cs
