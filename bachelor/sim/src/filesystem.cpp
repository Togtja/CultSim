#include "filesystem.h"

#include <physfs.h>
#include <spdlog/spdlog.h>

namespace cs
{
namespace fs
{
bool init(std::string_view project_name)
{
    if (!PHYSFS_init(project_name.data()))
    {
        return false;
        spdlog::error("failed to initialize PhysFS, {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }
    char p_name[] = "cultsim";
    char g_name[] = "g107";
    PHYSFS_addToSearchPath(PHYSFS_getPrefDir(g_name, p_name), 1);
    PHYSFS_setWriteDir(PHYSFS_getPrefDir(g_name, p_name));

    return true;
}
void deinit()
{
    if (!PHYSFS_deinit())
    {
        spdlog::error("failed to deinitialize PhysFS, {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }
}

std::string read_file(std::string_view rpath)
{
    auto file = PHYSFS_openRead(rpath.data());
    if (file == nullptr)
    {
        spdlog::warn("could not open {}", rpath);
        return {};
    }
    std::string ret(PHYSFS_fileLength(file), '\0');
    auto read_bytes = PHYSFS_readBytes(file, ret.data(), ret.length());
    if (read_bytes == 0)
    {
        spdlog::warn("the file: {} is empty", rpath);
    }
    if (read_bytes <= -1)
    {
        spdlog::error("the file: {} failed to read with error: {}", rpath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }
    return ret;
}
int write_file(std::string_view rpath)
{
}

bool exists(std::string_view rpath)
{
}

bool mkdir(std::string_view rpath)
{
}

bool mkfile(std::string_view rpath)
{
}
bool rename_file(std::string_view rpath_old, std::string_view rpath_new)
{
}
bool delete_file(std::string_view rpath)
{
}

} // namespace fs
} // namespace cs
