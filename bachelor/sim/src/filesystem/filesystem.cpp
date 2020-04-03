#include "filesystem.h"
#include "constants.h"

#include <physfs.h>
#include <spdlog/spdlog.h>

namespace cs::fs
{
bool init(std::string_view project_name)
{
    if (PHYSFS_init(project_name.data()) == 0)
    {
        spdlog::get("filesystem")->error("failed to initialize PhysFS, {}", get_errorstring());
        return false;
    }

    PHYSFS_setWriteDir(PHYSFS_getPrefDir(PROGRAM_TEAM.data(), PROGRAM_NAME.data()));
    spdlog::get("filesystem")->debug("write directory: {}", PHYSFS_getPrefDir(PROGRAM_TEAM.data(), PROGRAM_NAME.data()));

    PHYSFS_mount(PHYSFS_getPrefDir(PROGRAM_TEAM.data(), PROGRAM_NAME.data()), nullptr, 1);
    PHYSFS_mount("data.zip", nullptr, 1);

    return true;
}

bool deinit()
{
    if (static_cast<bool>(PHYSFS_deinit()))
    {
        return true;
    }
    spdlog::get("filesystem")->error("failed to deinitialize PhysFS, {}", get_errorstring());
    return false;
}

std::string read_file(std::string_view rpath)
{
    auto read_bytes = read_byte_file(rpath);
    return std::string(read_bytes.begin(), read_bytes.end());
}

std::vector<uint8_t> read_byte_file(std::string_view rpath)
{
    if (!is_file(rpath))
    {
        spdlog::get("filesystem")->warn("file: '{}' does not exist or is directory", rpath);
        return {};
    }

    auto file = PHYSFS_openRead(rpath.data());
    if (file == nullptr)
    {
        spdlog::get("filesystem")->error("could not open '{}' due to {}", rpath.data(), get_errorstring());
        return {};
    }

    auto ret        = std::vector<uint8_t>(PHYSFS_fileLength(file));
    auto read_bytes = PHYSFS_readBytes(file, ret.data(), ret.size());

    if (read_bytes == 0)
    {
        spdlog::get("filesystem")->warn("the file: '{}' is empty", rpath);
    }
    else if (read_bytes == -1)
    {
        spdlog::get("filesystem")->error("the file: '{}' failed to read with error: {}", rpath, get_errorstring());
    }

    PHYSFS_close(file);
    return ret;
}

uint64_t write_file(std::string_view rpath, const std::string& data)
{
    if (!exists(rpath))
    {
        spdlog::get("filesystem")->debug("file: '{}' does not exist", rpath);
        spdlog::get("filesystem")->info("creating file: '{}'", rpath);
    }
    else if (!is_file(rpath))
    {
        spdlog::get("filesystem")->warn("file: {} is not a file , but directory", rpath);
        return -1;
    }

    auto file = PHYSFS_openWrite(rpath.data());
    if (file == nullptr)
    {
        spdlog::get("filesystem")->error("could not write to '{}' due to {}", rpath, get_errorstring());
        return -1;
    }

    /** Note is a u64bit int, where the 64bit is failure */
    auto write_bytes = PHYSFS_writeBytes(file, data.data(), data.length());
    if (write_bytes == static_cast<PHYSFS_sint64>(data.length()))
    {
        spdlog::get("filesystem")->info("wrote file '{}'", rpath);
        PHYSFS_close(file);
        return write_bytes;
    }

    if (write_bytes == -1)
    {
        spdlog::get("filesystem")->error("the file: '{}' failed to write with error: {}", rpath, get_errorstring());
    }
    else
    {
        spdlog::get("filesystem")->error("the file: '{}' failed to write all bytes with error: {}", rpath, get_errorstring());
    }
    if (exists(rpath))
    {
        if (!delete_file(rpath))
        {
            spdlog::get("filesystem")->critical("filesystem corrupted by failed write");
            std::abort();
        }
    }
    return -1;
}

bool exists(std::string_view rpath)
{
    return static_cast<bool>(PHYSFS_exists(rpath.data()));
}

bool mkdir(std::string_view rpath)
{
    return static_cast<bool>(PHYSFS_mkdir(rpath.data()));
}

bool move_file(std::string_view rpath_old, std::string_view rpath_new)
{
    if (is_file(rpath_new))
    {
        spdlog::get("filesystem")->warn("attempt to overwrite file with move");
        return false;
    }

    /** Copy old to new and attempt to clean up */
    if (copy_file(rpath_old, rpath_new))
    {
        if (!delete_file(rpath_old))
        {
            spdlog::get("filesystem")->error("failed to delete old file during move");

            /** Attempt to recover by deleting new file to restore file system state */
            if (!delete_file(rpath_new))
            {
                spdlog::get("filesystem")->critical("failed to delete new files, file system corrupted");
                std::abort();
            }
            return false;
        }
        return true;
    }
    return false;
}

bool delete_file(std::string_view rpath)
{
    if (static_cast<bool>(PHYSFS_delete(rpath.data())))
    {
        return true;
    }
    spdlog::get("filesystem")->error("the file/dir: '{}' could not be deleted: {}", rpath, get_errorstring());
    return false;
}

bool copy_file(std::string_view rpath_old, std::string_view rpath_new, bool overwrite_existing)
{
    if (!is_file(rpath_old))
    {
        spdlog::get("filesystem")->warn("the file: '{}' does not exist or is not a file", rpath_old);
        return false;
    }

    if (rpath_new == rpath_old)
    {
        spdlog::get("filesystem")->warn("the old path is the same as the new path");
        return false;
    }

    if (!overwrite_existing && exists(rpath_new))
    {
        spdlog::get("filesystem")->warn("refused to overwrite existing file: '{}'", rpath_new);
        return false;
    }

    const auto data          = read_file(rpath_old);
    const auto bytes_written = write_file(rpath_new, data);
    spdlog::get("filesystem")->debug("read {} bytes, wrote {} bytes", data.length(), bytes_written);

    /** Attempt to write entire file contents and handle error if failed */
    if (bytes_written == data.length())
    {
        spdlog::get("filesystem")->info("successfully copied file");
        return true;
    }
    /** At this point a new file should have failed to be created,
     * if it has been created we delete to restore fs state */
    if (exists(rpath_new))
    {
        if (!delete_file(rpath_new))
        {
            spdlog::get("filesystem")->error("fail to delete new file during failed copy");
        }
    }
    else
    {
        spdlog::get("filesystem")->error("fail to create new file");
    }
    return false;
}

std::string_view get_errorstring()
{
    return PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
}

std::vector<std::string> list_directory(std::string_view rpath)
{
    if (!is_directory(rpath))
    {
        spdlog::get("filesystem")->warn("not a directory, returns empty vector");
        return {};
    }
    std::vector<std::string> files;
    auto* files_raw = PHYSFS_enumerateFiles(rpath.data());
    for (char** file = files_raw; *file != nullptr; ++file)
    {
        files.emplace_back(*file);
    }

    PHYSFS_freeList(files_raw);
    return files;
}

bool is_directory(std::string_view rpath)
{
    if (!exists(rpath))
    {
        return false;
    }
    PHYSFS_Stat stat{};
    if (static_cast<bool>(PHYSFS_stat(rpath.data(), &stat)))
    {
        return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
    }
    spdlog::get("filesystem")->warn("getting the Directory: '{}' stats failed with error: {}", get_errorstring());
    return false;
}

bool is_file(std::string_view rpath)
{
    if (!exists(rpath))
    {
        return false;
    }
    PHYSFS_Stat stat{};
    if (static_cast<bool>(PHYSFS_stat(rpath.data(), &stat)))
    {
        return stat.filetype == PHYSFS_FILETYPE_REGULAR;
    }
    spdlog::get("filesystem")->warn("getting the File: '{}' stats failed with error: {}", get_errorstring());
    return false;
}
} // namespace cs::fs
