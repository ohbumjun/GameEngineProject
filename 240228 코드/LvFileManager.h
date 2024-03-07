#pragma once
#ifndef __LV_FILE_MANAGER_H__
#define __LV_FILE_MANAGER_H__

#include "system/LvEvent.h"
#include "system/LvList.h"
#include "system/LvRingBuffer.h"
#include "system/LvString.h"
#include "system/experiment/LvConcurrentQueue.h"

#include "editor/private/project/LvFileInfo.h"
#include "system/LvFileSystem.h"

/* #include "editor/experiment/LvFileWatcher.h" */ namespace Lv
{
namespace Editor
{
struct LvFileWatcher;
}
} // namespace Lv
/* #include "editor/experiment/LvFileWatcher.h" */ namespace Lv
{
namespace Editor
{
struct LvFileWatcherEventHandler;
}
} // namespace Lv
/* #include "editor/experiment/LvFileWatcher.h" */ namespace Lv
{
namespace Editor
{
enum LvFileWatcherEventType : int;
}
} // namespace Lv
/* #include "editor/private/project/LvFileManagerEventArgs.h" */ namespace Lv
{
namespace Editor
{
namespace Project
{
enum class LvFileManagerChangeType : uint8;
}
} // namespace Editor
} // namespace Lv
/* #include "editor/private/project/LvFileManagerEventArgs.h" */ namespace Lv
{
namespace Editor
{
namespace Project
{
struct LvFileManagerEventArgs;
}
} // namespace Editor
} // namespace Lv
/* #include "editor/private/project/LvFileTable.h" */ namespace Lv
{
namespace Editor
{
namespace Project
{
class LvFileTable;
}
} // namespace Editor
} // namespace Lv
/* #include "editor/asset/LvAsset.h */ namespace Lv
{
namespace Editor
{
class LvAsset;
}
} // namespace Lv

// @Chan : This undef is for disabling Windows macro keywords
#ifdef CreateFile
#undef CreateFile
#endif

#ifdef CreateDirectory
#undef CreateDirectory
#endif

LV_NS_EDITOR_BEGIN

namespace Project
{
/**
	 * @brief LvEditorWindow::AttachProject가 완전히 성공한 이후에 호출할 수 있는 프로젝트 내부 파일 관리객체.
	 * @file #include "editor/private/project/LvFileManager.h"
	 */
class LV_EDITOR_API LvFileManager
{
public:
    template <typename T>
    //using Collection = System::LvConcurrentQueue<T>;
    using Collection = Concurrent::LvRingBuffer<T>;

    using Delegate = LvRawEvent<const LvFileManagerEventArgs &>::Delegate;
    using Dispatcher = LvRawEvent<const LvFileManagerEventArgs &>::Dispatcher;
    using FilesDispatcher =
        LvRawEvent<const LvList<LvFileManagerEventArgs> &>::Dispatcher;


    /**
		 * \brief [메인 쓰레드 이벤트] 프로젝트 폴더 내 변경된 '파일'의 이벤트 처리
		 */
    static LvRawEvent<const LvFileManagerEventArgs &> onDetectEvent;

    /**
		 * \brief [메인 쓰레드 이벤트] 프로젝트 폴더 내 변경된 '파일들'의 이벤트 처리
		 */
    static LvRawEvent<const LvList<LvFileManagerEventArgs> &>
        onDetectFilesEvent;

    /**
		 * \brief [메인 쓰레드 전용]
		 */
    static void Initialize(const char *projectPath);

    /**
		 * @brief _projectPath 변수만을 초기화 하는 함수
		 */
    static void InitializeProjectPath(const char *projectPath);

    /**
		 * \brief [메인 쓰레드 전용]
		 */
    static void Finalize();

    /**
		* @brief [메인 쓰레드 전용] FileWatcher를 시작
		*/
    static void StartFileWatcher();

    /**
		* @brief [메인 쓰레드 전용] FileWatcher를 종료
		*/
    static void StopFileWatcher();

    /**
		 * @return 로드된 프로젝트의 절대경로 반환
		 */
    static const LvString &GetProjectPath()
    {
        return _projectPath;
    }

    /**
		 * @return 프로젝트 폴더의 FileInfo를 반환
		 */
    static const LvFileInfo &GetProject();

    /**
		 * @return 프로젝트 폴더 내 Resources 폴더의 FileInfo를 반환
		 */
    static const LvFileInfo &GetProjectResources();

    /**
		 * @brief [메인 쓰레드 전용] 절대경로가 현재 프로젝트 폴더의 하위경로에 해당하는지 반환
		 * @param absolutePath 절대경로 ( "C:/projects/test/Resources/target.png" )
		 * @code
		 * LvFileManager::IsSubPath("C:/projects/test/Resources/target.png")
		 * @endcode
		 */
    static bool IsSubPath(const char *absolutePath);

    /**
		 * @brief [메인 쓰레드 전용] 절대경로를 프로젝트 폴더 기준의 상대경로로 변환
		 * @param absolutePath 대상 절대경로 ( "C:/projects/test/Resources/target.png" )
		 * @return 프로젝트 상대경로 ( "Resources/target.png" )
		 * @code
		 * LvFileManager::ToRelativePath("C:/projects/test/Resources/target.png")
		 * @endcode
		 */
    static LvString ToRelativePath(const LvString &absolutePath)
    {
        return ToRelativePath(absolutePath.c_str());
    }

    /**
		 * @brief [메인 쓰레드 전용] 절대경로를 프로젝트 폴더 기준의 상대경로로 변환
		 * @param absolutePath 대상 절대경로 ( "C:/projects/test/Resources/target.png" )
		 * @return 프로젝트 상대경로 ( "Resources/target.png" )
		 * @code
		 * LvFileManager::ToRelativePath("C:/projects/test/Resources/target.png")
		 * @endcode
		 */
    static LvString ToRelativePath(const char *absolutePath);

    /**
		 * @brief 프로젝트 폴더 기준의 상대경로를 절대경로로 변환
		 * @param relativePath 프로젝트 상대경로 ( "Resources/target.png" )
		 * @return 대상 절대경로 ( "C:/projects/test/Resources/target.png" )
		 * @code
		 * LvFileManager::ToAbsolutePath("Resources/target.png")
		 * @endcode
		 */
    static LvString ToAbsolutePath(const LvString &relativePath)
    {
        return ToAbsolutePath(relativePath.c_str());
    }

    /**
		 * @brief 프로젝트 폴더 기준의 상대경로를 절대경로로 변환
		 * @param relativePath 프로젝트 상대경로 ( "Resources/target.png" )
		 * @return 대상 절대경로 ( "C:/projects/test/Resources/target.png" )
		 * @code
		 * LvFileManager::ToAbsolutePath("Resources/target.png")
		 * @endcode
		 */
    static LvString ToAbsolutePath(const char *relativePath);

    /**
		 * @brief path file이 숨김 파일 인지
		 */
    static bool IsHidden(const char *target);

    /**
		 * @brief  source file path 가 숨김 파일 인지
		 */
    static bool IsSourceHiddenFromAbsolute(const char *absoluteSourcePath);

    /**
		 * @brief 파일 또는 디렉토리 존재 유무
		 * @code
		 * LvFileManager::IsExist("Resources/target.png")
		 * @endcode
		 */
    static bool IsExist(const LvString &target)
    {
        return IsExist(target.c_str());
    }

    /**
		 * @brief 파일 또는 디렉토리 존재 유무
		 * @code
		 * LvFileManager::IsExist("Resources/target.png")
		 * @endcode
		 */
    static bool IsExist(const char *target);

    /**
		 * @brief 파일 또는 디렉토리 존재 유무
		 * @code
		 * LvFileManager::IsExist("C:/ProjectResources/target.png")
		 * @endcode
		 */
    static bool IsExistFromAbsolute(const LvString &absoluteTarget)
    {
        return IsExistFromAbsolute(absoluteTarget.c_str());
    }

    /**
		 * @brief 파일 또는 디렉토리 존재 유무
		 * @code
		 * LvFileManager::IsExist("C:/ProjectResources/target.png")
		 * @endcode
		 */
    static bool IsExistFromAbsolute(const char *absoluteTarget);

    /**
		 * @brief 파일 또는 디렉토리 확인파일이 없는 경우 false
		 * @code
		 * LvFileManager::IsDirectory("Resources/target.png")
		 * @endcode
		 */
    static bool IsDirectory(const LvString &target)
    {
        return IsDirectory(target.c_str());
    }

    /**
		 * @brief 파일 또는 디렉토리 확인파일이 없는 경우 false
		 * @code
		 * LvFileManager::IsDirectory("Resources/target.png")
		 * @endcode
		 */
    static bool IsDirectory(const char *target);

    /**
		 * @brief 파일 또는 디렉토리 확인파일이 없는 경우 false
		 * @code
		 * LvFileManager::IsDirectory("C:/ProjectResources/target.png")
		 * @endcode
		 */
    static bool IsDirectoryFromAbsolute(const LvString &absoluteTarget)
    {
        return IsDirectoryFromAbsolute(absoluteTarget.c_str());
    }

    /**
		 * @brief 파일 또는 디렉토리 확인파일이 없는 경우 false
		 * @code
		 * LvFileManager::IsDirectory("C:/ProjectResources/target.png")
		 * @endcode
		 */
    static bool IsDirectoryFromAbsolute(const char *absoluteTarget);

    /**
		 * @brief 파일 스트림 반환
		 * @code
		 * LvFileManager::CreateFileStream("Resources/target.png")
		 * @endcode
		 */
    static LvFileStream CreateFileStream(const char *target,
                                         LvFileMode fileMode);

    /**
		 * @brief 파일 생성
		 * @code
		 * LvFileManager::CreateFile("Resources/target.png")
		 * @endcode
		 */
    static LvFileStream CreateFile(const char *target);

    /**
		 * @brief 파일 스트림 반환
		 * @code
		 * LvFileManager::CreateFileFromAbsolute("C:/Project/Resources/target.png", LvFileMode::OPEN_CREATE)
		 * @endcode
		 */
    static LvFileStream CreateFileStreamFromAbsolute(const char *target,
                                                     LvFileMode fileMode);

    /**
		 * @brief 파일 생성
		 * @code
		 * LvFileManager::CreateFileFromAbsolute("C:/Project/Resources/target.png")
		 * @endcode
		 */
    static LvFileStream CreateFileFromAbsolute(const char *target);

    /**
		 * @brief 디렉토리 생성
		 * @code
		 * LvFileManager::CreateDirectory("Resources/target")
		 * @endcode
		 */
    static bool CreateDirectory(const char *target);

    /**
		 * @brief 디렉토리 생성
		 * @code
		 * LvFileManager::CreateDirectoryFromAbsolute("C:/Project/Resources/Resources/target")
		 * @endcode
		 */
    static bool CreateDirectoryFromAbsolute(const char *target);

    /**
		 * @brief 파일 또는 디렉토리 제거
		 * @param target 제거할 대상
		 * @code
		 * LvFileManager::Delete("Resources/target.png")
		 * @endcode
		 */
    static bool Delete(const LvString &target)
    {
        return Delete(target.c_str());
    }

    /**
		 * @brief 파일 또는 디렉토리 제거
		 * @param target 제거할 대상
		 * @code
		 * LvFileManager::Delete("Resources/target.png")
		 * @endcode
		 */
    static bool Delete(const char *target);

    /**
		 * @brief 파일 또는 디렉토리 제거
		 * @param target 제거할 대상
		 * @code
		 * LvFileManager::DeleteFromAbsolute("C:/Project/Resources/Resources/target.png")
		 * @endcode
		 */
    static bool DeleteFromAbsolute(const char *target);

    /**
		 * @brief 파일 또는 디렉토리 복사
		 * @param source 복사할 대상
		 * @param target 복사될 경로
		 * @code
		 * LvFileManager::Copy("Resources/target.png", "Resources/A/target.png")
		 * @endcode
		 */
    static bool Copy(const char *source,
                     const char *target,
                     LvString *outCopiedPath = nullptr);

    /**
		 * @brief 절대경로를 통해 파일 또는 디렉토리 복사
		 * @param source 복사할 대상의 절대경로 (*열려있는 프로젝트 외부 파일을 대상으로 하는 경우 이벤트를 FileManager::LvChangeType::CREATE_RESOURCE 로 처리)
		 * @param target 복사될 절대경로
		 * @code
		 * LvFileManager::CopyFromAbsolute("C:/Project/Resources/target.png", "C:/Project/Resources/A/target.png")
		 * @endcode
		 */
    static bool CopyFromAbsolute(const char *source,
                                 const char *target,
                                 LvString *outCopiedPath = nullptr);

    /**
		 * @brief 파일 또는 디렉토리 이동
		 * @param source 이동할 대상
		 * @param target 이동될 경로
		 * @code
		 * LvFileManager::Move("Resources/target.png", "Resources/A/target.png")
		 * @endcode
		 */
    static bool Move(const char *source, const char *target);

    /**
		 * @brief 파일 또는 디렉토리 이름 변경 (* Move와 동일한 처리를 하지만 이벤트를 FileManager::LvChangeType::RENAME_RESOURCE 로 처리)
		 * @param source 변경할 대상
		 * @param target 변경 후 경로
		 * @code
		 * LvFileManager::Rename("Resources/target.png", "Resources/target2.png")
		 * @endcode
		 */
    static bool Rename(const char *source, const char *target);

    /**
		 * @brief 파일 또는 디렉토리 이름 변경 (* Move와 동일한 처리를 하지만 이벤트를 FileManager::LvChangeType::RENAME_RESOURCE 로 처리)
		 * @param source 변경할 대상
		 * @param target 변경 후 경로
		 * @code
		 * LvFileManager::RenameFromAbsolute("C:/Project/Resources/target.png", "C:/Project/Resources/target2.png")
		 * @endcode
		 */
    static bool RenameFromAbsolute(const char *source, const char *target);


    /**
		 * @brief 임의로 해당 파일이 수정되지 않았어도 강제로 import 하도록 할 때 사용(예 : hlsl 수정시 연관 lvs 강제 임포트)
		 *
		 */
    static bool Reimport(const char *source);

    /**
		 * \brief [메인 쓰레드 전용] 
		 * @param absolutePath FileInfo를 확인할 절대경로 (프로젝트 폴더 내부에 있는 경로여야 함.)
		 * @code
		 * LvFileManager::GetFileInfoAtPath("C://Project/Resources/target.png")
		 * @endcode
		 */
    static const LvFileInfo &GetFileInfoAtAbsolutePath(
        const char *absolutePath);

    /**
		 * \brief [메인 쓰레드 전용] 
		 * @param path FileInfo를 확인할 상대경로
		 * @code
		 * LvFileManager::GetFileInfoAtPath("Resources/target.png")
		 * @endcode
		 */
    static const LvFileInfo &GetFileInfoAtPath(const LvString &path)
    {
        return GetFileInfoAtPath(path.c_str());
    }

    /**
		 * \brief [메인 쓰레드 전용] 
		 * @param path FileInfo를 확인할 상대경로
		 * @code
		 * LvFileManager::GetFileInfoAtPath("Resources/target.png")
		 * @endcode
		 */
    static const LvFileInfo &GetFileInfoAtPath(const char *path);

    /**
		 * @brief [메인 쓰레드 전용] 절대경로의 하위 절대경로들을 반환
		 */
    static const LvList<LvString> &GetChildrenAtAbsolutePath(
        const char *absolutePath);

    /**
		 * @brief 해당 폴더에 같은 이름이 있는지 확인하고 있으면 " (숫자)" 를 덧붙혀서 반환
		 * @param name 파일이름
		 * @param parentPath 경로
		 * @param extensionWithDot '.' 이 포함된 확장자
		 * @details Resource폴더에서부터 상대경로를사용한다.
		 * 예를들어 Resource폴더내에 바로 이름을 조사하고싶으면 parentPath에 empty를 전달,
		 * Resource내에 human폴더를 조사하고싶으면 ParentPath에 "Human"을 전달
		 */
    static LvString GetUniqueName(const LvString &name,
                                  const LvString &parentPath,
                                  const LvString &extensionWithDot);

    /**
		 * @brief 복사될 경로와 이름을 전부 이어붙혀 문자열형태로 반환
		 * @param asset 복사될 에셋
		 * @param name 파일이름
		 * @param extensionWithDot '.' 이 포함된 확장자
		 */
    static LvString GetClonePath(const LvAsset *asset,
                                 const LvString &name,
                                 const LvString &extensionWithDot);

    /**
		 * @brief watcher의 기능을 잠시 멈춰놓습니다. 멈춰놓을동안 쌓인 이벤트는 pause가 풀리면 한꺼번에 수행됩니다. 
		 * 
		 */
    static void PauseWatcher();

    /**
		 * @brief pause된 watcher의 기능을 다시 동작합니다.
		 *
		 */
    static void ResumeWatcher();

    static const LvFileTable *GetTable()
    {
        return _table;
    }

private:
    struct WatchData
    {
        LvFileManagerChangeType type;
        LvString source;
        LvString destination;

        WatchData() : type(static_cast<LvFileManagerChangeType>(0))
        {
        }
        WatchData(LvFileManagerChangeType type,
                  const char *source,
                  const char *destination)
            : type(type), source(source),
              destination(destination ? LvString(destination) : "")
        {
        }
    };
    static bool isWatchingTargetPath(const char *absolutePath);
    static void addWatcher(const char *absolutePath);
    static void dispatch();
    static void record(LvFileManagerChangeType type,
                       const char *source,
                       const char *destination);
    static bool onWatch(LvFileWatcherEventType eventType,
                        const char *src,
                        const char *dst);
    static bool onWatch(LvFileManagerChangeType type,
                        const char *src,
                        const char *dst);

    static LvString _projectPath;
    static LvList<LvString> _watchingPaths;

    static LvFileInfo _root;
    static LvFileTable *_table;

    static bool _listening;
    static LvList<LvString> _watchingRootPath;
    static LvList<LvFileWatcher *> _watcher;
    static LvList<LvFileWatcherEventHandler *> _handler;

    static LvList<WatchData *> _dispatchDatas;
    static Collection<WatchData *> _watchDatas;
    static LvAtomic _pooling;

    static Dispatcher *_detectEvent;
    static FilesDispatcher *_detectFilesEvent;
};
} // namespace Project

LV_NS_EDITOR_END

#endif
