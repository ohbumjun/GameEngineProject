#pragma once

#include "FileModes.h"
#include "Hazel/Core/Serialization/Stream.h"

namespace Hazel
{


class FileStream : public Stream
{

public:
    FileStream(HANDLE file);

    FileStream(const char *path, FileOpenMode mode);

    FileStream(const char *path, FileOpenMode mode, FileAccessMode access);

    ~FileStream();

    int64 GetCurrentPos();

    void SetPos(int64 pos);

    void SerializeData(const void *ptr, size_t size);

    void DeserializeData(void *ptr, size_t size);

    void FlushToFile() const;

    void End();

    size_t GetDataLength() const;

private:
    HANDLE m_FileHandle;
    size_t m_Position = 0;
    size_t m_Length = 0;
};
}; // namespace HazelEditor