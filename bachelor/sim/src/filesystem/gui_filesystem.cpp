#include "gui_filesystem.h"
#include "filesystem.h"

namespace cs::fs
{
bool GuiFilesystem::FileExists(const ultralight::String16& path)
{
    return false;
}

bool GuiFilesystem::DeleteFile_(const ultralight::String16& path)
{
    return false;
}

bool GuiFilesystem::DeleteEmptyDirectory(const ultralight::String16& path)
{
    return false;
}

bool GuiFilesystem::MoveFile_(const ultralight::String16& old_path, const ultralight::String16& new_path)
{
    return false;
}

bool GuiFilesystem::GetFileSize(const ultralight::String16& path, int64_t& result)
{
    return false;
}

bool GuiFilesystem::GetFileSize(ultralight::FileHandle handle, int64_t& result)
{
    return false;
}

bool GuiFilesystem::GetFileMimeType(const ultralight::String16& path, ultralight::String16& result)
{
    return false;
}

bool GuiFilesystem::GetFileModificationTime(const ultralight::String16& path, time_t& result)
{
    return false;
}

bool GuiFilesystem::GetFileCreationTime(const ultralight::String16& path, time_t& result)
{
    return false;
}

ultralight::MetadataType GuiFilesystem::GetMetadataType(const ultralight::String16& path)
{
    return ultralight::MetadataType::kMetadataType_Unknown;
}

ultralight::String16 GuiFilesystem::GetPathByAppendingComponent(const ultralight::String16& path,
                                                                const ultralight::String16& component)
{
    return {};
}

bool GuiFilesystem::CreateDirectory_(const ultralight::String16& path)
{
    return false;
}

ultralight::String16 GuiFilesystem::GetHomeDirectory()
{
    return {};
}

ultralight::String16 GuiFilesystem::GetFilenameFromPath(const ultralight::String16& path)
{
    return {};
}

ultralight::String16 GuiFilesystem::GetDirectoryNameFromPath(const ultralight::String16& path)
{
    return {};
}

bool GuiFilesystem::GetVolumeFreeSpace(const ultralight::String16& path, uint64_t& result)
{
    return false;
}

int32_t GuiFilesystem::GetVolumeId(const ultralight::String16& path)
{
    return 0;
}

ultralight::Ref<ultralight::String16Vector> GuiFilesystem::ListDirectory(const ultralight::String16& path,
                                                                         const ultralight::String16& filter)
{
    return ultralight::String16Vector::Create();
}

ultralight::String16 GuiFilesystem::OpenTemporaryFile(const ultralight::String16& prefix, ultralight::FileHandle& handle)
{
    return {};
}

ultralight::FileHandle GuiFilesystem::OpenFile(const ultralight::String16& path, bool open_for_writing)
{
    return {};
}

void GuiFilesystem::CloseFile(ultralight::FileHandle& handle)
{
}

int64_t GuiFilesystem::SeekFile(ultralight::FileHandle handle, int64_t offset, ultralight::FileSeekOrigin origin)
{
    return 0LL;
}

bool GuiFilesystem::TruncateFile(ultralight::FileHandle handle, int64_t offset)
{
    return false;
}

int64_t GuiFilesystem::WriteToFile(ultralight::FileHandle handle, const char* data, int64_t length)
{
    return 0LL;
}

int64_t GuiFilesystem::ReadFromFile(ultralight::FileHandle handle, char* data, int64_t length)
{
    return 0LL;
}

bool GuiFilesystem::CopyFile_(const ultralight::String16& source_path, const ultralight::String16& destination_path)
{
    return false;
}
} // namespace cs::fs
