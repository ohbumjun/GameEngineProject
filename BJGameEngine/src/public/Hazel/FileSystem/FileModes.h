#pragma once

#include "hzpch.h"

#define INVALID_HANDLE ((HANDLE)(LONG_PTR)-1)
#define DIRECTORY_SEPARATOR_WCHAR L'\\'
#define DIRECTORY_SEPARATOR_CHAR '\\'

enum class FileAccessMode : uint8
{
    READ_ONLY,
    WRITE_ONLY,
    READ_WRITE
};

enum class FileOpenMode : uint8
{
    OPEN_CREATE, // 항상 열기. 없으면 만들어서 열기
    OPEN,        // 있을 때만 열기
    CREATE,      // 있어도 생성. 항상 생성
    APPEND,      // 없으면 생성
    NEW,         // 있을 때만 열기, 없으면 에러
};

enum class FilePosMode : uint8
{
    BEGIN,
    CURRENT,
    END
};
