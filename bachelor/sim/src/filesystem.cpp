#include "filesystem.h"
#include "constants.h"

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

    PHYSFS_setWriteDir(PHYSFS_getPrefDir(PROGRAM_TEAM.data(), PROGRAM_NAME.data()));

    PHYSFS_mount(PHYSFS_getPrefDir(PROGRAM_TEAM.data(), PROGRAM_NAME.data()), nullptr, 1);
    PHYSFS_mount("data.zip", nullptr, 1);

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
    if (exists(rpath_new))
    {
        spdlog::warn("attempt to overwrite file with move");
        return false;
    }

    /** Copy old to new and attempt to clean up */
    if (copy_file(rpath_old, rpath_new))
    {
        if (!delete_file(rpath_old))
        {
            spdlog::error("failed to delete old file during move");

            /** Attempt to recover by deleting new file to restore file system state */
            if (!delete_file(rpath_new))
            {
                spdlog::critical("failed to delete new files, file system corrupted");
                std::abort();
            }
            return false;
        }
        return true;
    }

    /** At this point a new file is created, but is incomplete, so delete to restore fs state */
    if (exists(rpath_new))
    {
        if (!delete_file(rpath_new))
        {
            spdlog::error("failed to delete new file during move");
        }
    }
    return false;
}

bool delete_file(std::string_view rpath)
{
    return static_cast<bool>(PHYSFS_delete(rpath.data()));
}

bool copy_file(std::string_view rpath_old, std::string_view rpath_new, bool overwrite_existing)
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

    if (!overwrite_existing && exists(rpath_new))
    {
        spdlog::warn("refused to overwrite existing file: {}", rpath_new);
        return false;
    }

    const auto data          = read_file(rpath_old);
    const auto bytes_written = write_file(rpath_new, data.data());
    spdlog::debug("read {} bytes, wrote {} bytes", data.length(), bytes_written);

    /** Attempt to write entire file contents and handle error if failed */
    if (static_cast<uint64_t>(bytes_written) == data.length())
    {
        spdlog::info("successfully copied file");
        return true;
    }
    /** Error states below */
    else if (bytes_written >= 0)
    {
        if (!delete_file(rpath_new))
        {
            spdlog::critical("filesystem corrupted by copy");
            std::abort();
        }
    }

    spdlog::error("could not copy file");
    return false;
}

std::vector<std::string> list_directory(std::string_view rpath)
{
    if (!PHYSFS_isDirectory(rpath.data()))
    {
        spdlog::warn("not a directory, returns empty vector");
        return {};
    }
    std::vector<std::string> files;
    auto files_raw = PHYSFS_enumerateFiles(rpath.data());
    for (char* file = *files_raw; file; file = *++files_raw)
    {
        files.emplace_back(file);
    }
    PHYSFS_freeList(files_raw);
    return files;
}

} // namespace fs
} // namespace cs
