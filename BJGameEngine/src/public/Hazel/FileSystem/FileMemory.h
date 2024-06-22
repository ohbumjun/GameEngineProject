#pragma once

#include "FileModes.h"

namespace Hazel
{


class FileMemory
{

public:
    FileMemory(HANDLE file);

    FileMemory(const char *path, FileOpenMode mode);

    FileMemory(const char *path, FileOpenMode mode, FileAccessMode access);

    ~FileMemory();

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