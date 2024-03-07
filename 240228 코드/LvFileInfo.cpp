#include "editor/private/project/LvFileInfo.h"
#include "LvPrecompiled.h"

#include "LvFileTable.h"
#include "system/LvLog.h"
#include "system/util/LvMDx.h"

#include "editor/private/project/LvFileManager.h"
#include "engine/LvEngine.h"

using namespace Lv;
using namespace Lv::Engine;
using namespace Lv::Editor::Project;

LV_NS_EDITOR_BEGIN

LvFileInfo::LvFileInfo()
    : name(""), path(""), absolutePath(""), extension(""), isDirectory(false),
      isInf(false), isValid(false)
{
}

LvFileInfo::LvFileInfo(const char *abs, const LvFileTable *baseTable)
{
    absolutePath = lv_path_normalize(abs);
    name = lv_path_name(absolutePath.c_str());
    if (name.Contains("."))
    {
        extension = lv_path_extension_to_lower(absolutePath.c_str());
    }

    path = baseTable->ToRelativePath(absolutePath.c_str());
    if (!path.IsEmpty())
    {
        path = path.SubString(1, path.Length() - 1);
    }

    isDirectory = baseTable->isDirectoryExist(abs);
    isInf = !extension.IsEmpty() && extension == ".inf";
    isValid = true;
}

LvFileInfo &LvFileInfo::operator=(const LvFileInfo &rhs)
{
    name = rhs.name;
    path = rhs.path;
    absolutePath = rhs.absolutePath;
    extension = rhs.extension;
    isDirectory = rhs.isDirectory;
    isInf = rhs.isInf;
    isValid = rhs.isValid;

    return *this;
}

const LvFileInfo &LvFileInfo::FindChild(const char *childName) const
{
    const LvList<LvString> &childAbsolutePaths = GetChildAbsolutePaths();
    for (uint64 i = 0, max = childAbsolutePaths.Count(); i < max; i++)
    {
        LvString name = lv_path_name(childAbsolutePaths[i].c_str());
        if (name == childName)
        {
            return LvFileManager::GetFileInfoAtAbsolutePath(
                childAbsolutePaths[i].c_str());
        }
    }

    const static LvFileInfo info;
    return info;
}

const LvList<LvString> &LvFileInfo::GetChildAbsolutePaths() const
{
    LV_CHECK_MAIN_THREAD();
    return LvFileManager::GetChildrenAtAbsolutePath(absolutePath.c_str());
}

LvString LvFileInfo::GetResourcesRelativePath() const
{
    return ToResourcesRelativePathAtAbsolutePath(path.c_str());
}

LvString LvFileInfo::ToResourcesRelativePathAtAbsolutePath(
    const char *absolutePath)
{
    return lv_path_without_directory(LvEngineContext::Directories::resources,
                                     absolutePath);
}

LvString LvFileInfo::ToResourcesRelativePathAtRelativePath(
    const char *relativePath)
{
    return ToResourcesRelativePathAtAbsolutePath(
        LvFileManager::ToAbsolutePath(relativePath).c_str());
}

LvString LvFileInfo::ToAbsolutePathAtResourcesRelativePath(
    const char *relativePath)
{
    return LvFileManager::ToAbsolutePath(
        lv_path_combine(LvEngineContext::Directories::resources, relativePath)
            .c_str());
}

LvString LvFileInfo::ToRelativePathAtResourcesRelativePath(
    const char *relativePath)
{
    return lv_path_combine(LvEngineContext::Directories::resources,
                           relativePath);
}

static LvString createUniquePath(const char *targetPath,
                                 std::function<bool(const char *)> isDirectory,
                                 std::function<bool(const char *)> isExist)
{
    LvString parent = lv_path_parent(targetPath);
    if (!isExist(parent.c_str()))
    {
        // 대상 경로의 부모 경로가 존재하지 않으면 생성하지 않음
        LV_LOG(warning, "Not found parent path (%s)", targetPath);
        return LvString::Empty();
    }

    if (!isExist(targetPath))
    {
        // 이미 존재하지 않는 경로라면 해당 경로를 그대로 반환
        return targetPath;
    }

    bool isFile = !isDirectory(targetPath);

    LvString name = lv_path_name_without_extension(targetPath);
    LvString extension = lv_path_extension(targetPath);

    // 디렉토리의 경우 .으로 끝나는 부분을 확장자로 보지 않으며 전체 이름으로 간주함
    if (!isFile)
    {
        name.Append(extension);
    }

    LvString path;

    size_t max = -1;
    size_t number = 0;
    while (number < max)
    {
        if (isFile)
        {
            path = lv_path_combine(parent.c_str(),
                                   LvString::Format("%s_%u%s",
                                                    name.c_str(),
                                                    number,
                                                    extension.c_str())
                                       .c_str());
        }
        else
        {
            path = lv_path_combine(
                parent.c_str(),
                LvString::Format("%s_%u", name.c_str(), number).c_str());
        }

        if (!isExist(path.c_str()))
        {
            return path;
        }
        number++;
    }

    // size_t를 다 채울때까지 돌았다는 것이며 이 경우 해당 함수를 더 고도화 할 필요가 있음.
    LV_LOG(crash, "Impossible..");
    return LvString::Empty();
}

LvString LvFileInfo::CreateUniqueRelativePath(const char *relativePath)
{
    return createUniquePath(
        relativePath,
        [](const char *path) { return LvFileManager::IsDirectory(path); },
        [](const char *path) { return LvFileManager::IsExist(path); });
}

LvString LvFileInfo::CreateUniqueAbsolutePath(const char *absolutePath)
{
    return createUniquePath(
        absolutePath,
        [](const char *target) { // IsDirectory
            return lv_directory_exist(target);
        },
        [](const char *target) { // IsExist
            return lv_file_exist(target) || lv_directory_exist(target);
        });
}

LV_NS_EDITOR_END