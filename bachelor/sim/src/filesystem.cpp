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
        spdlog::error("failed to initialize PhysFS, {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        return false;
    }

    char p_name[] = "cultsim";
    char g_name[] = "g107";
    PHYSFS_mount(PHYSFS_getPrefDir(g_name, p_name), nullptr, 1);

    /* TODO: Set up our zip archive to be mounted here */

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
    if (!exists(rpath))
    {
        spdlog::warn("file: {} does not exist", rpath);
        return {};
    }

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
    else if (read_bytes <= -1)
    {
        spdlog::error("the file: {} failed to read with error: {}", rpath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }
    PHYSFS_close(file);
    return ret;
}

int64_t write_file(std::string_view rpath, const std::string& data)
{
    if (!exists(rpath))
    {
        spdlog::debug("file: {} does not exist", rpath);
        spdlog::info("creating file: {}", rpath);
    }

    auto file = PHYSFS_openWrite(rpath.data());
    if (file == nullptr)
    {
        spdlog::warn("could not write to {}", rpath);
        return -1;
    }
    auto write_bytes = PHYSFS_writeBytes(file, data.data(), data.length());
    if (write_bytes == 0)
    {
        spdlog::info("nothing written to file: {}", rpath);
    }
    else if (write_bytes <= -1)
    {
        spdlog::error("the file: {} failed to write with error: {}", rpath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }
    PHYSFS_close(file);
    return write_bytes;
}

bool exists(std::string_view rpath)
{
    return PHYSFS_exists(rpath.data());
}

bool mkdir(std::string_view rpath)
{
    return PHYSFS_mkdir(rpath.data());
}

bool move_file(std::string_view rpath_old, std::string_view rpath_new)
{
    if (copy_file(rpath_old, rpath_new))
    {
        if (!delete_file(rpath_old))
        {
            spdlog::error("failed to delete old file during renaming");
        }
        return true;
    }

    spdlog::error("failed to rename file");

    if (exists(rpath_new))
    {
        if (!delete_file(rpath_new))
        {
            spdlog::error("failed to delete new file during renaming");
        }
    }
    return false;
}

bool delete_file(std::string_view rpath)
{
    return PHYSFS_delete(rpath.data());
}

bool copy_file(std::string_view rpath_old, std::string_view rpath_new)
{
    if (!exists(rpath_old))
    {
        spdlog::warn("the file: {} does not exist", rpath_old);
        return false;
    }
    if (rpath_new == rpath_old)
    {
        spdlog::warn("the old path is the same as the new path");
        return false;
    }

    auto data  = read_file(rpath_old);
    auto bytes = write_file(rpath_new, data.data());

    spdlog::debug("read {} bytes, wrote {} bytes", data.length(), bytes);

    if (static_cast<uint64_t>(bytes) == data.length())
    {
        spdlog::info("succesfully copied file");
        return true;
    }

    return false;
}

} // namespace fs
} // namespace cs
