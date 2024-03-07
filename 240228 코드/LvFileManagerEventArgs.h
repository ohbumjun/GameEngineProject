#pragma once
#ifndef __LV_FILE_MANAGER_EVENT_ARGS_H__
#define __LV_FILE_MANAGER_EVENT_ARGS_H__

LV_NS_EDITOR_BEGIN

namespace Project
{
/**
	 * @file #include "editor/private/project/LvFileManagerEventArgs.h"
	 */
enum class LvFileManagerChangeType : uint8
{
    NONE,

    CREATE_RESOURCE, /// Resources 폴더 내부의 파일 또는 디렉토리가 생성 됨
    REMOVE_RESOURCE, /// Resources 폴더 내부의 파일 또는 디렉토리가 제거 됨
    MODIFY_RESOURCE, /// Resources 폴더 내부의 파일 또는 디렉토리가 수정 됨
    MOVE_RESOURCE, /// Resources 폴더 내부의 파일 또는 디렉토리가 LvFileManager에 의해 이동 됨, OS의 감지가 정상적으로 되고 있는지 보장되지 않음.
    RENAME_RESOURCE, /// Resources 폴더 내부의 파일 또는 디렉토리의 LvFileManager에 의해 이름이 수정 됨
    COPY_RESOURCE, /// Resources 폴더 내부의 파일 또는 디렉토리가 LvFileManager에 의해 복사 됨

    MAX
};

/**
	 * @file #include "editor/private/project/LvFileManagerEventArgs.h"
	 */
struct LV_EDITOR_API LvFileManagerEventArgs
{
    LvFileManagerChangeType type = LvFileManagerChangeType::NONE;
    const char *absoluteSourcePath = nullptr;
    const char *absoluteTargetPath = nullptr;

    LvFileManagerEventArgs()
    {
    }
    LvFileManagerEventArgs(LvFileManagerChangeType type,
                           const LvString &absoluteSourcePath,
                           const LvString &absoluteTargetPath)
        : type(type), absoluteSourcePath(absoluteSourcePath.c_str()),
          absoluteTargetPath(absoluteTargetPath.c_str())
    {
    }
};
} // namespace Project

LV_NS_EDITOR_END

#endif
