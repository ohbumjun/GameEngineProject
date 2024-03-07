#include "editor/private/project/LvFileTable.h"
#include "LvPrecompiled.h"

#include "system/LvLog.h"
//

#include "engine/LvEngineAllocator.h"
#include "system/LvFileSystem.h"

#pragma region system

#pragma endregion

using namespace Lv;
using namespace Lv::Editor::Project;

LV_NS_EDITOR_BEGIN

void LvFileTable::Clear()
{
    if (_root)
    {
        releaseFileInfo(_root);

        _root = nullptr;
        _rootAbsolutePath.Clear();
        _targetPaths.Clear();
        _infos.Clear();
        _absolutePathToInfo.Clear();
    }
}

LvFileTable::Info *LvFileTable::createInstance(const char *parentAbsolutePath,
                                               const char *absolutePath,
                                               LvList<LvString> *creates)
{
    // 현재 대상 경로가 루트거나 추적 대상 경로여야 생성
    if (_rootAbsolutePath != absolutePath && !isTargetPath(absolutePath))
        return nullptr;

    // 부모경로가 더 낮은 인덱스에 담김
    if (creates)
        creates->Add(absolutePath);

    Info *parent = nullptr != parentAbsolutePath
                       ? getFileInfoAtAbsolutePath(parentAbsolutePath)
                       : nullptr;
    Info *info = getFileInfoAtAbsolutePath(absolutePath);
    if (nullptr == info)
    {
        info = new (LV_ENGINE_SAFE_ALLOC(Info))
            Info(this, parent, absolutePath, creates);
        _infos.Add(info);
        // LV_LOG(warning, "[Alloc] %u (%u) / %s", ++count, _absolutePathToInfo.Count(), info->data.path.c_str());
    }

    return info;
}

const LvFileInfo &LvFileTable::CreateRoot(const char *rootAbsolutePath,
                                          const LvList<LvString> &targetPaths)
{
    const LvString rootName = getPathName(rootAbsolutePath);

    _targetPaths = targetPaths;
    _rootAbsolutePath = rootAbsolutePath;
    _root = createInstance(nullptr, rootAbsolutePath, nullptr);
    return _root->data;
}

bool LvFileTable::isTargetPath(const char *absolutePath)
{
    if (_targetPaths.Count() == 0)
        return true;

    for (uint64 i = 0, max = _targetPaths.Count(); i < max; i++)
    {
        if (0 == strncmp(absolutePath,
                         _targetPaths[i].c_str(),
                         _targetPaths[i].Length()))
        {
            return true;
        }
    }
    return false;
}

void LvFileTable::releaseFileInfo(Info *info)
{
    LV_CHECK(nullptr != info, "Wrong value");

    const LvList<LvString> &childs = info->GetChildAbsolutePaths();
    if (0 < info->GetChildCount())
    {
        for (int64 i = static_cast<int64>(info->GetChildCount()) - 1; 0 <= i;
             --i)
        {
            releaseFileInfo(getFileInfoAtAbsolutePath(childs[i].c_str()));
        }
    }

    deregister(info);

    // LV_LOG(warning, "[Free] %u / %s", --count, info->data.path.c_str());
    info->~Info();
    LV_ENGINE_SAFE_FREE(info);
}

LvFileTable::~LvFileTable()
{
    if (_root)
    {
        releaseFileInfo(_root);
    }
}

void LvFileTable::regist(Info *info)
{
    LV_CHECK(!_absolutePathToInfo.ContainsKey(info->data.absolutePath),
             "Already registed (%s)",
             info->data.path.c_str());

    const LvString key = toNormalize(info->data.absolutePath.c_str());
    _absolutePathToInfo.Add(key, info);
}

void LvFileTable::deregister(Info *info)
{
    _infos.Remove(info);

    const LvString key = toNormalize(info->data.absolutePath.c_str());
    _absolutePathToInfo.Remove(key);
}

void LvFileTable::deregister(const char *absolutePath)
{
    deregister(_absolutePathToInfo[absolutePath]);
}

const LvFileInfo &LvFileTable::GetFileInfoAtAbsolutePath(
    const char *absolutePath) const
{
    const Info *info = getFileInfoAtAbsolutePath(absolutePath);
    if (nullptr != info)
    {
        return info->data;
    }
    static const LvFileInfo result;
    return result;
}

const LvList<LvString> &LvFileTable::GetChildAtAbsolutePaths(
    const char *absolutePath) const
{
    const Info *info = getFileInfoAtAbsolutePath(absolutePath);
    if (nullptr != info)
    {
        return info->GetChildAbsolutePaths();
    }

    static const LvList<LvString> result;
    return result;
}

bool LvFileTable::CreateRecord(const char *absolutePath,
                               LvList<LvString> *creates)
{
    const bool existInfo = nullptr != getFileInfoAtAbsolutePath(absolutePath);

    // 이미 있으면 기록하지 않음
    if (existInfo)
        return false;

    const char *rootAbsolutePath = _root->data.absolutePath.c_str();
    const size_t length = strlen(rootAbsolutePath);

#ifndef LV_DEVELOPE_RENDER
    // Root 경로가 맞지 않으면 무시 (Resources)
    if (0 != strncmp(rootAbsolutePath, absolutePath, length))
    {
        LV_LOG(warning,
               "LvFileTable::CreateRecord Fail - Wrong path %s (Need root path "
               "%s)",
               absolutePath,
               rootAbsolutePath);
        return false;
    }
#endif

    // 파일이나 디렉토리가 없으면 기록하지 않음
    if (!isFileExist(absolutePath) && !isDirectoryExist(absolutePath))
    {
        // 해당 로그 주석 처리 이유 : https://jira.com2us.com/jira/browse/CSECOTS-4827
        // LV_LOG(warning, "LvFileTable::CreateRecord Fail - Not found path %s", absolutePath);
        return false;
    }

    const LvString parentAbsolutePath = getPathParent(absolutePath);

    const bool existParent =
        nullptr != getFileInfoAtAbsolutePath(parentAbsolutePath.c_str());
    if (!existParent)
    {
        // 상위 FileInfo가 존재하지 않으면 생성, 이때 자식을 모두 생성하므로 현재 경로를 추가로 생성하지 않음
        const bool result = CreateRecord(parentAbsolutePath.c_str(), creates);
        if (!result)
        {
            LV_LOG(
                warning,
                "LvFileTable::CreateRecord Fail - Fail create parent info %s",
                absolutePath);
        }
        return result;
    }

    // 현재 경로에 대한 정보 생성
    const Info *current =
        createInstance(parentAbsolutePath.c_str(), absolutePath, creates);
    if (nullptr != current)
    {
        return true;
    }

    // 추적 대상 경로가 아닌경우
    // LV_LOG(warning, "LvFileTable::CreateRecord Pass - Not a path to watching %s", absolutePath);
    return false;
}

bool LvFileTable::ModifyRecord(const char *absolutePath,
                               LvList<LvString> *creates)
{
    const auto info = getFileInfoAtAbsolutePath(absolutePath);

    if (nullptr == info)
        return CreateRecord(absolutePath, creates);

    if (!info->data.isDirectory)
        return true;

    const auto list = getDirectoryLists(absolutePath);

    if (info->GetChildCount() == list.Count())
        return true;

    RemoveRecord(absolutePath, false);

    return CreateRecord(absolutePath, creates);
}

LvString LvFileTable::ToAbsolutePath(const char *relativePath) const
{
    return lv_path_combine(_root->data.absolutePath.c_str(), relativePath);
}

LvString LvFileTable::ToRelativePath(const char *absolutePath) const
{
    LvString result = absolutePath;

    if (0 != strncmp(_rootAbsolutePath.c_str(),
                     absolutePath,
                     _rootAbsolutePath.Length()))
        LV_LOG(warning,
               "Converting an absolute path that does not match the root (root "
               ": %s target : %s)",
               _rootAbsolutePath.c_str(),
               absolutePath);

    result.Remove(0, _rootAbsolutePath.Length());
    return result;
}

LvFileTable::Info *LvFileTable::getFileInfoAtAbsolutePath(
    const char *absolutePath) const
{
    const LvString key = toNormalize(absolutePath);
    if (!_absolutePathToInfo.ContainsKey(key))
        return nullptr;
    return _absolutePathToInfo[key];
}

LvString LvFileTable::toNormalize(const char *path) const
{
    LvString result = LvString::ToLower(path);
    result.Replace("\\", "/");
    if (result[result.Length() - 1] == '/')
    {
        result = result.SubString(0, result.Length() - 1);
    }

    return result;
}

bool LvFileTable::RemoveRecord(const char *absolutePath,
                               const bool checkExistFile)
{
    // 파일이나 디렉토리가 있으면 기록하지 않음
    if (checkExistFile &&
        (isFileExist(absolutePath) || isDirectoryExist(absolutePath)))
    {
        //LV_LOG(warning, "LvFileTable::RemoveRecord Pass - %s is exist.", absolutePath);
        return false;
    }

    Info *info = getFileInfoAtAbsolutePath(absolutePath);
    // 이미 지워져 있으면 기록하지 않음
    if (nullptr == info)
    {
        //LV_LOG(warning, "LvFileTable::RemoveRecord Pass - Already removed %s", absolutePath);
        return false;
    }

    const LvList<LvString> &children = info->GetChildAbsolutePaths();
    if (0 < info->GetChildCount())
    {
        for (int64 i = static_cast<int64>(info->GetChildCount()) - 1; 0 <= i;
             --i)
        {
            RemoveRecord(children[i].c_str(), checkExistFile);
        }
    }
    deregister(info);

    // LV_LOG(warning, "[Free] %u / %s", --count, info->data.path.c_str());
    info->~Info();
    LV_ENGINE_SAFE_FREE(info);

    return true;
}

bool LvFileTable::MoveRecord(const char *oldAbsolutePath,
                             const char *newAbsolutePath,
                             LvList<LvString> *creates)
{
    const bool result = CreateRecord(newAbsolutePath, creates);
    RemoveRecord(oldAbsolutePath, true);
    return result;
}

LvFileTable::Info::Info(LvFileTable *table,
                        Info *parent,
                        const char *absolutePath,
                        LvList<LvString> *creates)
    : data(LvFileInfo{absolutePath, table})
{
    _table = table;
    _parent = parent;

    _table->regist(this);

    if (data.isDirectory)
    {
        Lv::LvList<LvString> childs =
            table->getDirectoryLists(data.absolutePath.c_str());
        for (uint64 i = 0, max = childs.Count(); i < max; i++)
        {
            LvString childAbsolutePath =
                table->pathCombine(data.absolutePath.c_str(),
                                   childs[i].c_str());

            _table->createInstance(data.absolutePath.c_str(),
                                   childAbsolutePath.c_str(),
                                   creates);
        }
    }

    if (nullptr != _parent)
    {
        LV_CHECK(!_parent->_childAbsolutePaths.Contains(data.absolutePath),
                 "Already regist file info");

        _parent->_childAbsolutePaths.Add(data.absolutePath);
    }
}

LvFileTable::Info::~Info()
{
    if (nullptr != _parent)
    {
        _parent->_childAbsolutePaths.Remove(data.absolutePath);
    }
}

uint64 LvFileTable::Info::GetChildCount() const
{
    return _childAbsolutePaths.Count();
}

const LvList<LvString> &LvFileTable::Info::GetChildAbsolutePaths() const
{
    return _childAbsolutePaths;
}

const LvFileTable::Info *LvFileTable::Info::GetChild(uint64 index) const
{
    return _table->getFileInfoAtAbsolutePath(
        _childAbsolutePaths[index].c_str());
}

const LvFileTable::Info *LvFileTable::Info::FindChild(
    const char *childName) const
{
    for (uint64 i = 0, max = _childAbsolutePaths.Count(); i < max; i++)
    {
        const Info *info = GetChild(i);
        if (info->data.name == childName)
        {
            return info;
        }
    }
    return nullptr;
}

LV_NS_EDITOR_END
