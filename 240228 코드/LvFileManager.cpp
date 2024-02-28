#include "LvPrecompiled.h"
#include "editor/private/project/LvFileManager.h"

#include "system/LvLog.h"
#include "system/LvMemory.h"

#include "engine/LvEngine.h"
#include "engine/thirdparty/imgui/imgui.h"
#include "engine/thread/LvDispatchQueue.h"
#include "engine/util/LvBuiltInUtil.h"

#include "editor/LvEditorContext.h"
#include "editor/window/LvEditorWindow.h"
#include "editor/asset/LvAssetDatabase.h"
#include "editor/experiment/LvFileWatcher.h"
#include "editor/experiment/LvFileWatcherEvents.h"
#include "editor/experiment/LvFileWatcherEventType.h"
#include "editor/private/project/LvFileInfo.h"
#include "editor/private/project/LvFileTable.h"
#include "editor/asset/LvAsset.h"

#include "editor/panel/LvNativePanel.h"
#include "editor/LvSelection.h"
#include "editor/private/project/LvFileManagerEventArgs.h"
#include <cerrno>

#include "editor/asset/LvAssetExtension.h"

using namespace Lv;
using namespace Lv::Engine;
using namespace Lv::Editor::Project;
using namespace Lv::Editor::Project::FileManager;

#ifdef _DEBUG
#define DEBUG_FILE_MANAGER
#endif

#pragma region system

#pragma endregion

#pragma region DEBUG

#if defined DEBUG_FILE_MANAGER
#include "system/LvReflection.h"
#include "editor/attribute/LvMenuItemAttribute.h"

LV_NS_EDITOR_BEGIN

class LvFileManagerDebugPanel : public LvNativePanel
{
public:
	LV_FORCEINLINE LvTypeId GetType() const override { return LvReflection::GetTypeId<LvFileManagerDebugPanel>(); }

private:
	static LvFileManager::Collection<LvString> _logQueue;
	static constexpr int MAX_ARRAY = 1024;

public:
	static void Enqueue(const char* log)
	{
		_logQueue.Enqueue(log);
	}

protected:
	void onGUI() override
	{
		bool opened = true;
		ImGui::Begin("File Manager Debugger", &opened);
		{
			while (_logQueue.Count())
			{
				LvString log;
				if (_logQueue.Dequeue(log))
				{
					_logs.Add(log);
					if (MAX_ARRAY < _logs.Count())
					{
						_logs.RemoveAt(0);
					}
				}
			}

			if (ImGui::Button("Clear"))
			{
				_logs.Clear();
			}
			ImGui::SameLine();
			ImGui::Text("%zu/%d", _logs.Count(), MAX_ARRAY);
			ImGui::BeginChild("log", ImVec2(0, ImGui::GetContentRegionAvail().y), true,
				ImGuiWindowFlags_HorizontalScrollbar);
			for (uint64 i = 0, max = _logs.Count(); i < max; i++)
			{
				ImGui::Text("%s", _logs[i].c_str());
			}
			ImGui::EndChild();

			if (opened == false)
			{
				Close();
			}
		}
		ImGui::End();
	}

private:
	LvList<LvString> _logs;
};

LvFileManager::Collection<LvString> LvFileManagerDebugPanel::_logQueue;

namespace // anonymous namespace
{
	void openPanel(const char* menuName)
	{
		LvFileManagerDebugPanel* panel = lv_editor_get_window()->CreatePanel<LvFileManagerDebugPanel>();
		if (nullptr != panel)
		{
			panel->Open();
		}
	}
}

LV_REFLECTION_REGIST
{
	LvReflection::RegistBase<LvFileManagerDebugPanel, LvNativePanel>();
	LvReflection::RegistAttribute<LvMenuItemAttribute>(LvAttributeTarget::METHOD,
													   LvMenuItemAttribute("Tools/Debug/File Manager", openPanel));
}

LV_NS_EDITOR_END

#endif
#pragma endregion

/*

	- Editor를 통해 프로젝트 폴더를 열면 내부 Resources 폴더를 감시한다.
	- OS의 디렉토리 내부 변경점은 File Watcher를 통해 수집한다.
	- 수집된 이벤트를 통해 잘못된 이벤트인지 검증후 File Table에 기록한다.

*/

LV_NS_EDITOR_BEGIN

LvString LvFileManager::_projectPath = "";
LvList<LvString> LvFileManager::_watchingPaths;

LvFileInfo LvFileManager::_root;
static LvFileTable table;

LvFileTable* LvFileManager::_table = nullptr;

bool LvFileManager::_listening = true;
LvList<LvString> LvFileManager::_watchingRootPath;
LvList<LvFileWatcher*> LvFileManager::_watcher;
LvList<LvFileWatcherEventHandler*> LvFileManager::_handler;

LvList<LvFileManager::WatchData*> LvFileManager::_dispatchDatas;
LvFileManager::Collection<LvFileManager::WatchData*> LvFileManager::_watchDatas;
LvAtomic LvFileManager::_pooling;

LvRawEvent<const LvFileManagerEventArgs&>::Dispatcher* LvFileManager::_detectEvent = nullptr;
LvRawEvent<const LvFileManagerEventArgs&> LvFileManager::onDetectEvent(&_detectEvent);

LvRawEvent<const LvList<LvFileManagerEventArgs>&>::Dispatcher* LvFileManager::_detectFilesEvent = nullptr;
LvRawEvent<const LvList<LvFileManagerEventArgs>&> LvFileManager::onDetectFilesEvent(&_detectFilesEvent);

void LvFileManager::Initialize(const char* projectPath)
{
	LV_CHECK_MAIN_THREAD();

	_projectPath = lv_path_normalize(projectPath);
	_table = &table;

	_table->isFileExist = lv_file_exist;
	_table->isDirectoryExist = lv_directory_exist;
	_table->getPathParent = lv_path_parent;
	_table->getDirectoryLists = static_cast<LvList<LvString>(*)(const char*)>(lv_directory_lists);
	_table->pathCombine = static_cast<LvString(*)(const char*, const char*)>(lv_path_combine);
	_table->getPathName = lv_path_name;

	addWatcher(projectPath);
	
	LV_LOG(debug, "LvFileManager::Initialize %s", projectPath);

	_watchingPaths.Add(lv_path_normalize(lv_path_combine(_projectPath, LvEditorContext::Directories::bundle).c_str()));
	_watchingPaths.Add(lv_path_normalize(lv_path_combine(_projectPath, LvEditorContext::Directories::library).c_str()));
	_watchingPaths.Add(lv_path_normalize(lv_path_combine(_projectPath, LvEditorContext::Directories::project).c_str()));
	_watchingPaths.Add(lv_path_normalize(lv_path_combine(_projectPath, LvEditorContext::Directories::resources).c_str()));
	_watchingPaths.Add(lv_path_normalize(lv_path_combine(_projectPath, LvEditorContext::Directories::engine).c_str()));
	_watchingPaths.Add(lv_path_normalize(lv_path_combine(_projectPath, LvEditorContext::Directories::settings).c_str()));
	_watchingPaths.Add(lv_path_normalize(lv_path_combine(_projectPath, LvEditorContext::Directories::solution).c_str()));
	_watchingPaths.Add(lv_path_normalize(lv_path_combine(_projectPath, LvEditorContext::Directories::program).c_str()));
	_watchingPaths.Add(lv_path_normalize(lv_path_combine(_projectPath, LvEditorContext::Directories::builtin).c_str()));
	
	_root = _table->CreateRoot(_projectPath.c_str(), _watchingPaths);

	lv_atomic_set(&_pooling, 0);
}

void Project::LvFileManager::InitializeProjectPath(const char* projectPath)
{
	_projectPath = lv_path_normalize(projectPath);
}

void LvFileManager::Finalize()
{
	LV_CHECK_MAIN_THREAD();

	for (const auto data: _dispatchDatas)
	{
		data->~WatchData();
		LV_ENGINE_FREE(data);
	}

	StopFileWatcher();
	//lv_file_watcher_end(_watcher);

	for(size_t i = 0, max = _handler.Count(); i < max; ++i)

	{
		if(_handler[i] != nullptr)
		{
			LV_ENGINE_DELETE(_handler[i]);
			_handler[i] = nullptr;
		}
		_watcher[i] = nullptr;
		_watchingRootPath[i] = "";
	}

	if(_table != nullptr)
	{
		// LV_ENGINE_DELETE(_table);
		table.Clear();
		_table = nullptr;
	}

	_watchingPaths.Clear();
	_projectPath = "";
}

void LvFileManager::StartFileWatcher()
{
	LV_CHECK_MAIN_THREAD();

	for (size_t i = 0, max = _handler.Count(); i < max; ++i)
	{
		LV_ASSERT(_watcher[i] == nullptr, "watcher is already running");

		constexpr LvFileWatcherCreateFlags flag = LV_FILEWATCHER_RECURSIVE;
		constexpr LvFileWatcherEventType event = LV_FILEWATCHER_EVENT_ALL;

		_watcher[i] = lv_file_watcher_start(flag, event, _watchingRootPath[i].c_str(), _handler[i]);
	}
}

void LvFileManager::StopFileWatcher()
{
	LV_CHECK_MAIN_THREAD();

	for (size_t i = 0, max = _watcher.Count(); i < max; ++i)
	{
		if (_watcher[i] != nullptr)
		{
			lv_file_watcher_end(_watcher[i]);
			_watcher[i] = nullptr;
		}
	}
}

bool LvFileManager::CreateDirectoryFromAbsolute(const char* target)
{
	if (lv_directory_create(target))
	{
		onWatch(LvFileManagerChangeType::CREATE_RESOURCE, target, nullptr);
		return true;
	}
	return false;
}

bool LvFileManager::DeleteFromAbsolute(const char* target)
{
	if (lv_directory_exist(target))
	{
		if (lv_directory_remove(target))
		{
			onWatch(LvFileManagerChangeType::REMOVE_RESOURCE, target, nullptr);
			return true;
		}
		LV_LOG(warning, "Failed directory delete %s", target);
	}
	else if (lv_file_exist(target))
	{
		const int returnCode = lv_file_delete(target);
		if (0 == returnCode)
		{
			onWatch(LvFileManagerChangeType::REMOVE_RESOURCE, target, nullptr);
			return true;
		}

		const int errorCode = errno;
		// https://docs.microsoft.com/ko-kr/cpp/c-runtime-library/reference/remove-wremove?view=msvc-160
		// 현재 해당 파일이 파일이 열려있거나 읽기 전용인 경우 삭제를 실패할 수 있음.
		LV_LOG(warning, "Failed file delete [%d] %s : %s", returnCode, target, lv_print_errno(errorCode));
	}
	return false;
}

bool LvFileManager::RenameFromAbsolute(const char* source, const char* target)
{
	if (0 != strcmp(source, target))
	{
		const bool samePath = lv_path_compare(source, target);

		if (lv_directory_exist(source))
		{
			if (samePath || !lv_directory_exist(target))
			{
				bool recursive = false;
				const size_t length = strlen(source);
				if (length <= strlen(target) && LV_DIRECTORY_SEPARATOR_CHAR == target[length])
				{
					char* const sub = new char[length];
#if defined(__WIN32__)
					strncpy_s(sub, length, target, _TRUNCATE);
#else
					strlcpy(sub, target, length);
#endif
					sub[length - 1] = 0;
					if (0 == strcmp(sub, source)) // 여기서 같다면, 상위 폴더가 내부 폴더로 순환이동을 시도하는 것
					{
						recursive = true;
					}
					delete[] sub;
				}

				if (!recursive)
				{
					if (lv_directory_rename(source, target))
					{
						onWatch(LvFileManagerChangeType::MOVE_RESOURCE, source, target);
						return true;
					}
					LV_LOG(warning, "Failed directory rename %s -> %s", source, target);
				}
				else
				{
					LV_LOG(warning, "Failed cannot move to child path %s", target);
				}
			}
			else
			{
				LV_LOG(warning, "Failed directory does exist %s", target);
			}
		}
		else if (lv_file_exist(source))
		{
			if (samePath || !lv_file_exist(target))
			{
				if (lv_file_rename(source, target))
				{
					onWatch(LvFileManagerChangeType::MOVE_RESOURCE, source, target);
					return true;
				}
				LV_LOG(warning, "Failed file rename %s -> %s", source, target);
			}
			else
			{
				LV_LOG(warning, "Failed file does exist %s", target);
			}
		}
		else
		{
			LV_LOG(warning, "Failed path does not exist %s", target);
		}
	}
	else
	{
		LV_LOG(warning, "Failed wrong path %s", target);
	}

	return false;
}

bool LvFileManager::Reimport(const char* source)
{
	onWatch(LvFileManagerChangeType::MODIFY_RESOURCE, source, nullptr);
	return true;
}

bool LvFileManager::CopyFromAbsolute(const char* source, const char* target, LvString* outCopiedPath)
{
	const bool import = !isWatchingTargetPath(source);
	_listening = false;
	if (lv_directory_exist(source))
	{
		if (!lv_directory_exist(target))
		{
			if (0 != strcmp(source, target))
			{
				if (lv_directory_copy(target, source))
				{
					_listening = true;
					if (import)
					{
						onWatch(LvFileManagerChangeType::CREATE_RESOURCE, target, "");
					}
					else
					{
						onWatch(LvFileManagerChangeType::COPY_RESOURCE, source, target);
					}

					if (outCopiedPath != nullptr)
					{
						*outCopiedPath = target;
					}

					return true;
				}
			}
		}
	}
	else if (lv_file_exist(source))
	{
		int32 index = 1;
		LvString targetStrName = target;

		const LvString targetExtension = lv_path_extension(target);
		LvString targetPathWithoutExtension = target;
		if (!targetExtension.IsEmpty())
		{
			targetPathWithoutExtension.Replace(targetExtension.c_str(), "");
		}

		while (lv_file_exist(targetStrName.c_str()))
		{
			targetStrName = LvString::Format("%s (%d)%s", targetPathWithoutExtension.c_str(), index++, targetExtension.c_str());
		}

		if (lv_file_copy(targetStrName.c_str(), source))
		{
			_listening = true;

			if (import)
			{
				onWatch(LvFileManagerChangeType::CREATE_RESOURCE, target, "");
			}
			else
			{
				onWatch(LvFileManagerChangeType::COPY_RESOURCE, source, target);
			}

			if (outCopiedPath != nullptr)
			{
				*outCopiedPath = targetStrName;
			}

			return true;
		}
	}

	_listening = true;
	return false;
}

bool LvFileManager::IsSubPath(const char* absolutePath)
{
	LV_CHECK_MAIN_THREAD();
	const LvString result(absolutePath);
	const uint64 length = _projectPath.Length() + 1; // '/' 포함
	if (length < result.Length())
	{
		LvString parentPath = result.SubString(0, _projectPath.Length());
		if (parentPath == _projectPath)
		{
			return true;
		}
	}
	return false;
}

LvString LvFileManager::ToRelativePath(const char* absolutePath)
{
	LV_CHECK_MAIN_THREAD();
	const LvString result(absolutePath);
	const uint64 length = _projectPath.Length() + 1; // '/' 포함
	if (length < result.Length())
	{
		LvString parentPath = result.SubString(0, _projectPath.Length());
		if (parentPath == _projectPath)
		{
			return result.SubString(length, result.Length() - length);
		}
	}
	return LvString();
}

LvString LvFileManager::ToAbsolutePath(const char* relativePath)
{
	return lv_path_combine(_projectPath.c_str(), relativePath);
}

bool LvFileManager::IsHidden(const char* target)
{
	const LvString absoluteTarget = lv_path_combine(_projectPath, target);

	LvFileAttribute attr;
	lv_file_attribute(absoluteTarget.c_str(), attr);

	return attr.hidden;
}

bool LvFileManager::IsSourceHiddenFromAbsolute(const char* absoluteSourcePath)
{
	if (lv_directory_exist(absoluteSourcePath))
	{
		return false;
	}

	const LvString absoluteTarget = absoluteSourcePath;

	LvFileAttribute attr;
	lv_file_attribute(absoluteTarget.c_str(), attr);

	return attr.hidden;
}

bool LvFileManager::IsExist(const char* target)
{
	const LvString absoluteTarget = lv_path_combine(_projectPath, target);
	return IsExistFromAbsolute(absoluteTarget.c_str());
}

bool LvFileManager::IsExistFromAbsolute(const char* absoluteTarget)
{
	return lv_directory_exist(absoluteTarget) || lv_file_exist(absoluteTarget);
}

bool LvFileManager::IsDirectory(const char* target)
{
	const LvString absoluteTarget = lv_path_combine(_projectPath, target);
	return IsDirectoryFromAbsolute(absoluteTarget.c_str());
}

bool LvFileManager::IsDirectoryFromAbsolute(const char* absoluteTarget)
{
	return lv_directory_exist(absoluteTarget);
}

LvFileStream LvFileManager::CreateFileStream(const char* target, const LvFileMode fileMode)
{
	const LvString absoluteTarget = lv_path_combine(_projectPath, target);

	return CreateFileStreamFromAbsolute(absoluteTarget.c_str(), fileMode);
}

LvFileStream LvFileManager::CreateFile(const char* target)
{
	const LvString absoluteTarget = lv_path_combine(_projectPath, target);

	return CreateFileFromAbsolute(absoluteTarget.c_str());
}

LvFileStream LvFileManager::CreateFileStreamFromAbsolute(const char* target, LvFileMode fileMode)
{
	const bool exist = lv_file_exist(target);

	LvFileStream stream(target, fileMode);

	if (!exist)
	{
		switch (fileMode)
		{
		case LvFileMode::APPEND:
		case LvFileMode::CREATE:
		case LvFileMode::NEW:
		case LvFileMode::OPEN_CREATE:
		case LvFileMode::TRUNCATE:
			onWatch(LvFileManagerChangeType::CREATE_RESOURCE, target, nullptr);
			break;

		case LvFileMode::OPEN:
		default: break;
		}
	}
	return stream;
}

LvFileStream LvFileManager::CreateFileFromAbsolute(const char* target)
{
	LvFileStream stream(lv_file_create(target));
	onWatch(LvFileManagerChangeType::CREATE_RESOURCE, target, nullptr);
	return stream;
}

bool LvFileManager::CreateDirectory(const char* target)
{
	if (nullptr == target) return false;

	const LvString absoluteTarget = lv_path_combine(_projectPath, target);

	return CreateDirectoryFromAbsolute(absoluteTarget.c_str());
}

bool LvFileManager::Delete(const char* target)
{
	if (nullptr == target) return false;

	const LvString absoluteTarget = lv_path_combine(_projectPath, target);

	return DeleteFromAbsolute(absoluteTarget.c_str());
}

bool LvFileManager::Copy(const char* source, const char* target, LvString* outCopiedPath)
{
	if (nullptr == source) return false;

	const LvString absoluteSource = lv_path_combine(_projectPath, source);
	const LvString absoluteTarget = lv_path_combine(_projectPath, target);

	return CopyFromAbsolute(absoluteSource.c_str(), absoluteTarget.c_str(), outCopiedPath);
}

bool LvFileManager::Move(const char* source, const char* target)
{
	if (nullptr == source) return false;

	const LvString absoluteSource = lv_path_combine(_projectPath, source);
	const LvString absoluteTarget = lv_path_combine(_projectPath, target);

	return RenameFromAbsolute(absoluteSource.c_str(), absoluteTarget.c_str());
}

bool LvFileManager::Rename(const char* source, const char* target)
{
	if (nullptr == source) return false;

	const LvAssetType assetType = LvAssetExtension::GetAssetTypeByExtension(lv_path_extension(target));
	if (LvAssetType::SCENE == assetType)
	{
		if (LvAssetDatabase::IsSameSceneNameExist(target))
		{
			// 씬은 하나의 프로젝트에서 같은 이름이 여러개 있을 수 없음
			LV_LOG(warning, "A name that already exists. %s", target);
			return false;
		}
	}

	const LvString absoluteSource = lv_path_combine(_projectPath, source);
	const LvString absoluteTarget = lv_path_combine(_projectPath, target);

	return RenameFromAbsolute(absoluteSource.c_str(), absoluteTarget.c_str());
}

const LvFileInfo& LvFileManager::GetFileInfoAtAbsolutePath(const char* absolutePath)
{
	LV_CHECK_MAIN_THREAD();
	return _table->GetFileInfoAtAbsolutePath(absolutePath);
}

const LvFileInfo& LvFileManager::GetFileInfoAtPath(const char* path)
{
	LV_CHECK_MAIN_THREAD();
	return _table->GetFileInfoAtAbsolutePath(ToAbsolutePath(path).c_str());
}

const LvList<LvString>& LvFileManager::GetChildrenAtAbsolutePath(const char* absolutePath)
{
	LV_CHECK_MAIN_THREAD();
	return _table->GetChildAtAbsolutePaths(absolutePath);
}

LvString LvFileManager::GetUniqueName(const LvString& name, const LvString& parentPath, const LvString& extensionWithDot)
{
	LvString result;

	const LvString parent = LvString::IsNullOrEmpty(parentPath) ? LvEngineContext::Directories::resources : lv_path_combine(LvEngineContext::Directories::resources, parentPath);

	//여기서 들어오는 name은 항상 확장자가 붙어있다.
	//따라서 확장자를 제외한 이름만 이용해야함.
	const LvString& nameWithoutEx = lv_path_name_without_extension(name.c_str());

	int index = 0;
	do
	{
		result = index == 0 ?
			LvString::Format("%s%s", nameWithoutEx.c_str(), extensionWithDot.c_str())
			: LvString::Format("%s (%d)%s", nameWithoutEx.c_str(), index, extensionWithDot.c_str());
		result.Trim();
		++index;
	} while (IsExist(lv_path_combine(parent, result).c_str()));

	return result;
}

LvString LvFileManager::GetClonePath(const LvAsset* asset, const LvString& name, const LvString& extensionWithDot)
{
	const LvAsset* mainAsset = LvAssetDatabase::GetAssetByGuid(asset->uuid.GetGuid());
	const LvString parentPath = lv_path_parent(mainAsset->path.c_str());
	const LvString uniquePath = GetUniqueName(name, parentPath, extensionWithDot);

	return  LvString::IsNullOrEmpty(parentPath) ? uniquePath : lv_path_combine(parentPath, uniquePath);
}

void LvFileManager::PauseWatcher()
{
	for (size_t i = 0, max = _watchingRootPath.Count(); i < max; ++i)
	{
		lv_file_watcher_set_pause(_watchingRootPath[i].c_str(), true);
	}
}


void LvFileManager::ResumeWatcher()
{
	for (size_t i = 0, max = _watchingRootPath.Count(); i < max; ++i)
	{
		lv_file_watcher_set_pause(_watchingRootPath[i].c_str(), false);
	}
}


bool LvFileManager::isWatchingTargetPath(const char* absolutePath)
{
	for (uint64 i = 0, max = _watchingPaths.Count(); i < max; i++)
	{
		if (0 == strncmp(absolutePath, _watchingPaths[i].c_str(), _watchingPaths[i].Length()))
		{
			return true;
		}
	}
	return false;
}

void LvFileManager::addWatcher(const char* absolutePath)
{
	LvFileWatcherEventHandler* handler = LV_ENGINE_NEW(LvFileWatcherEventHandler);
	_handler.Add(handler);
	_watcher.Add(nullptr);
	_watchingRootPath.Add(absolutePath);
	handler->callback = onWatch;
}

const LvFileInfo& LvFileManager::GetProject()
{
	return _root;
}

const LvFileInfo& LvFileManager::GetProjectResources()
{
	return _root.FindChild(LvEngineContext::Directories::resources);
}

bool LvFileManager::onWatch(LvFileWatcherEventType eventType, const char* src, const char* dst)
{
	LV_CHECK(nullptr != src, "wrong source path");
	if (!isWatchingTargetPath(src)) return false;

	LvString log;
	LvFileManagerChangeType type = LvFileManagerChangeType::NONE;

	switch (eventType)
	{
	case LV_FILEWATCHER_EVENT_CREATE:
		type = LvFileManagerChangeType::CREATE_RESOURCE;
		log = "Create";
		break;
	case LV_FILEWATCHER_EVENT_REMOVE:
		type = LvFileManagerChangeType::REMOVE_RESOURCE;
		log = "Remove";
		break;
	case LV_FILEWATCHER_EVENT_MODIFY:
		type = LvFileManagerChangeType::MODIFY_RESOURCE;
		log = "Modify";
		break;
	case LV_FILEWATCHER_EVENT_MOVE:
		type = LvFileManagerChangeType::MOVE_RESOURCE;
		log = "Move";
		break;
	case LV_FILEWATCHER_EVENT_ALL:
		log = "All";
		break;
	case LV_FILEWATCHER_EVENT_BUFFER_OVERFLOW:
		log = "Overflow";
		break;
	}

#if defined DEBUG_FILE_MANAGER
	log.AppendFormat("\t%s\t%s", src, dst);
	LvFileManagerDebugPanel::Enqueue(log.c_str());
#endif

	return onWatch(type, src, dst);
}

bool LvFileManager::onWatch(LvFileManagerChangeType type, const char* src, const char* dst)
{
	switch (type)
	{
	case LvFileManagerChangeType::MOVE_RESOURCE:
		if (lv_path_parent(src) == lv_path_parent(dst))
		{
			return onWatch(LvFileManagerChangeType::RENAME_RESOURCE, src, dst);
		}
		break;
	case LvFileManagerChangeType::NONE:
	case LvFileManagerChangeType::CREATE_RESOURCE:
	case LvFileManagerChangeType::REMOVE_RESOURCE:
	case LvFileManagerChangeType::MODIFY_RESOURCE:
	case LvFileManagerChangeType::COPY_RESOURCE:
	case LvFileManagerChangeType::MAX:
	case LvFileManagerChangeType::RENAME_RESOURCE:
		break;
	}

	_watchDatas.Enqueue(new(lv_malloc(sizeof(WatchData)))WatchData(type, src, dst));

	if (0 == lv_atomic_get(&_pooling))
	{
		LvDispatchQueue::Main().Async([&]
			{
				lv_atomic_set(&_pooling, 1);
				while (_watchDatas.Count())
				{
					WatchData* watchData;
					if (!_watchDatas.Dequeue(watchData))
					{
						continue;
					}
					record(watchData->type, watchData->source.c_str(), watchData->destination.c_str());
				}
				dispatch();
				lv_atomic_set(&_pooling, 0);
			});
	}

	return true;
}

//static const char* s_typeStrings[] =
//{
//	"CREATE", /// Resources 폴더 내부의 파일 또는 디렉토리가 생성 됨
//	"REMOVE", /// Resources 폴더 내부의 파일 또는 디렉토리가 제거 됨
//	"MODIFY", /// Resources 폴더 내부의 파일 또는 디렉토리가 수정 됨
//	"  MOVE", /// Resources 폴더 내부의 파일 또는 디렉토리가 LvFileManager에 의해 이동 됨, OS의 감지가 정상적으로 되고 있는지 보장되지 않음.
//	"RENAME", /// Resources 폴더 내부의 파일 또는 디렉토리의 LvFileManager에 의해 이름이 수정 됨
//	"  COPY", /// Resources 폴더 내부의 파일 또는 디렉토리가 LvFileManager에 의해 복사 됨
//};

void LvFileManager::record(const LvFileManagerChangeType type, const char* source, const char* destination)
{
	bool result = false;
	LvList<LvString> creates;
	if (_table != nullptr)
	{
		switch (type)
		{
		case LvFileManagerChangeType::CREATE_RESOURCE:
		{
			result = _table->CreateRecord(source, &creates);
		}
		break;

		case LvFileManagerChangeType::REMOVE_RESOURCE:
		{
			result = _table->RemoveRecord(source, true);
		}
		break;

		case LvFileManagerChangeType::MODIFY_RESOURCE:
		{
			result = _table->ModifyRecord(source, &creates);
		}
		break;

		case LvFileManagerChangeType::MOVE_RESOURCE:
		case LvFileManagerChangeType::RENAME_RESOURCE:
		{
			if (0 != strcmp(source, destination) && lv_path_compare(source, destination))
			{
				// 대소문자만 변경하는 경우
				const bool removed = _table->RemoveRecord(source, false);
				const bool created = _table->CreateRecord(destination, nullptr);
				result = removed && created;
			}
			else if (lv_path_parent(source) == lv_path_parent(destination))
			{
				// 이름을 변경한 경우
				result = _table->MoveRecord(source, destination, nullptr);
			}
			else
			{
				// 다른 경로로 이동시키는 경우
				result = _table->MoveRecord(source, destination, &creates);
			}
		}
		break;

		case LvFileManagerChangeType::COPY_RESOURCE:
		{
			//LvList<LvString> copies;
			result = _table->CreateRecord(destination, &creates);
		}
		break;

		case LvFileManagerChangeType::NONE:
		case LvFileManagerChangeType::MAX:
			break;
		}
	}

	if (result)
	{
		const bool hasDestination = LvFileManagerChangeType::MOVE_RESOURCE == type ||
			LvFileManagerChangeType::RENAME_RESOURCE == type ||
			LvFileManagerChangeType::COPY_RESOURCE == type;

		if (0 < creates.Count())
		{
			const bool isDirectroy = hasDestination && lv_directory_exist(destination);
			for (size_t i = 0, max = creates.Count(); i < max; ++i)
			{
				if (hasDestination)
				{
					LvString sourceParent = isDirectroy ? source : lv_path_parent(source);
					LvString destParent = isDirectroy ? destination : lv_path_parent(destination);
					LvString original = creates[i];
					original.Replace(destParent.c_str(), sourceParent.c_str());
					_dispatchDatas.Add(new(lv_malloc(sizeof(WatchData)))WatchData(type, original.c_str(), creates[i].c_str()));
				}
				else
				{
					_dispatchDatas.Add(new(lv_malloc(sizeof(WatchData)))WatchData(type, creates[i].c_str(), nullptr));
				}
			}
		}
		else
		{
			_dispatchDatas.Add(new(lv_malloc(sizeof(WatchData)))WatchData(type, source, destination));
		}
	}
}

void LvFileManager::dispatch()
{
	LvList<WatchData*> watchDatas;
	LvList<LvFileManagerEventArgs> eventArgses;
	for(WatchData * watchData :_dispatchDatas)
	{
		LvFileManagerEventArgs args(watchData->type, watchData->source, watchData->destination);

		// 이벤트를 받는 대상의 순서를 알 수 없는 관계로 
		// LvSelection에 담겨있는 유효하지 않은 uuid 부터 해제
		bool changed = false;
		LvList<LvString> uuids(LvSelection::activeUuids());
		for (int64 j = static_cast<int64>(uuids.Count()) - 1; 0 <= j; --j)
		{
			if (nullptr == LvAssetDatabase::GetAssetByUuid(LvUuid::Parse(uuids[j])))
			{
				uuids.RemoveAt(j);
				changed = true;
			}
		}
		if (changed)
		{
			LvSelection::activeUuids = uuids;
		}

		//const char* types[] {
		//	"  NONE",
		//	"CREATE",
		//	"REMOVE",
		//	"MODIFY",
		//	"  MOVE",
		//	"RENAME",
		//	"  COPY",
		//};
		// LV_LOG(debug, "[%s] %s %s", types[(int)args.type], args.absoluteSourcePath, args.absoluteTargetPath);
		_detectEvent->Dispatch(args);

		watchDatas.Add(watchData);
		eventArgses.Add(args);

		//watchData->~WatchData();
		//lv_free(watchData);
	}
	_dispatchDatas.Clear();

	if (!eventArgses.IsEmpty())
	{
		_detectFilesEvent->Dispatch(eventArgses);
	}

	for (const auto& each : watchDatas)
	{
		each->~WatchData();
		lv_free(each);
	}
}

#undef DEBUG_FILE_MANAGER

LV_NS_EDITOR_END
