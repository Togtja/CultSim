#pragma once

#include <Ultralight/platform/FileSystem.h>
#include <Ultralight/RefPtr.h>

namespace cs::fs
{
class GuiFilesystem : public ultralight::FileSystem
{
private:
public:
    bool FileExists(const ultralight::String16& path) override;
    bool DeleteFile_(const ultralight::String16& path) override;
    bool DeleteEmptyDirectory(const ultralight::String16& path) override;
    bool MoveFile_(const ultralight::String16& old_path, const ultralight::String16& new_path) override;
    bool GetFileSize(const ultralight::String16& path, int64_t& result) override;
    bool GetFileSize(ultralight::FileHandle handle, int64_t& result) override;
    bool GetFileMimeType(const ultralight::String16& path, ultralight::String16& result) override;
    bool GetFileModificationTime(const ultralight::String16& path, time_t& result) override;
    bool GetFileCreationTime(const ultralight::String16& path, time_t& result) override;
    ultralight::MetadataType GetMetadataType(const ultralight::String16& path) override;
    ultralight::String16 GetPathByAppendingComponent(const ultralight::String16& path,
                                                     const ultralight::String16& component) override;
    bool CreateDirectory_(const ultralight::String16& path) override;
    ultralight::String16 GetHomeDirectory() override;
    ultralight::String16 GetFilenameFromPath(const ultralight::String16& path) override;
    ultralight::String16 GetDirectoryNameFromPath(const ultralight::String16& path) override;
    bool GetVolumeFreeSpace(const ultralight::String16& path, uint64_t& result) override;
    int32_t GetVolumeId(const ultralight::String16& path) override;
    ultralight::Ref<ultralight::String16Vector> ListDirectory(const ultralight::String16& path,
                                                              const ultralight::String16& filter) override;
    ultralight::String16 OpenTemporaryFile(const ultralight::String16& prefix, ultralight::FileHandle& handle) override;
    ultralight::FileHandle OpenFile(const ultralight::String16& path, bool open_for_writing) override;
    void CloseFile(ultralight::FileHandle& handle) override;
    int64_t SeekFile(ultralight::FileHandle handle, int64_t offset, ultralight::FileSeekOrigin origin) override;
    bool TruncateFile(ultralight::FileHandle handle, int64_t offset) override;
    int64_t WriteToFile(ultralight::FileHandle handle, const char* data, int64_t length) override;
    int64_t ReadFromFile(ultralight::FileHandle handle, char* data, int64_t length) override;
    bool CopyFile_(const ultralight::String16& source_path, const ultralight::String16& destination_path) override;
};

} // namespace cs::fs
