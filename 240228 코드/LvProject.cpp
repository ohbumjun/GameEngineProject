#include "LvPrecompiled.h"
#include "editor/project/LvProject.h"

#include "system/LvLog.h"
#include "system/LvReflection.h"
#include "system/LvFileSystem.h"
#include "system/LvNativeWindow.h"
#include "system/LvSystemContext.h"
#include "system/LvProcess.h"
#include "system/LvEnum.h"
#include "system/experiment/LvCallStack.h"
#include "system/util/LvAST.h"

#include "engine/thirdparty/imgui/imgui.h"
#include "engine/thirdparty/imgui/imgui_internal.h"
#include "engine/LvEngine.h"
#include "engine/LvSceneGraph.h"

#include "engine/asset/LvBundleRepository.h"
#include "engine/archive/LvBinaryArchive.h"
#include "engine/LvBuildSettings.h"
#include "engine/archive/LvJsonDomArchive.h"
#include "engine/thread/LvDispatchQueue.h"
#include "engine/LvEngineAllocator.h"
#include "engine/LvApplicable.h"
#include "engine/LvCachedPropertyPath.h"
#include "engine/LvManagedJobSystem.h"
#include "engine/archive/LvYamlArchive.h"
#include "engine/private/LvBuiltInDistribute.h"
#include "engine/LvUserSetting.h"
#include "engine/renderer/LvRenderServer.h"
#include "engine/LvBuiltInResource.h"

#include "engine/util/LvBuiltInUtil.h"


#include "editor/LvEditorContext.h"
#include "editor/window/LvEditorWindow.h"
#include "editor/window/LvWorkspace.h"
#include "editor/coreclr/LvCoreclrBridge.h"
#include "editor/scene/LvEditorSceneManager.h"
#include "editor/LvEditorSceneGraph.h"

#include "editor/private/project/LvFileManager.h"
#include "editor/panel/popup/LvMessagePanel.h"
#include "editor/asset/LvAssetDatabase.h"
#include "editor/private/asset/LvAssetDatabaseInternal.h"
#include "editor/asset/LvAsset.h"
#include "editor/command/LvProjectCommands.h"
#include "editor/LvEditorScene.h"
#include "editor/LvProjectPreferences.h"
#include "editor/LvSelection.h"



#include "editor/window/layout/LvPanelController.h"
#include "editor/private/project/LvFileManagerEventArgs.h"
#include "editor/asset/LvShaderAsset.h"
#include "editor/asset/LvHLSLAsset.h"
#include "editor/asset/LvTextAsset.h"
#include "editor/private/asset/info/LvTextAssetInfo.h"

#include "editor/project/LvProjectContext.h"
#include "editor/bundle/LvBundleManifestManager.h"
#include "editor/experiment/LvPlatformType.h"
#include "editor/private/LvBuiltInDistribute.h"
#include "editor/project/LvProjectGenerator.h"
#include "editor/private/LvDependency.h"


using namespace Lv;

using namespace Lv::Engine;
using namespace Lv::Editor::Project;
using namespace Lv::Editor::Project::FileManager;
//using namespace Lv::Editor::MonoBridge;
using namespace Lv::Editor::CoreCLR;
using namespace Lv::Editor::Command;
using namespace Lv::Editor::Popup;

LV_NS_EDITOR_BEGIN


namespace //anonymous namespace
{
	class ScriptChangeAction
	{
	public:

		void Prepare()
		{
			LvAsset* asset = LvEditorSceneManager::GetActiveAsset();
			if (nullptr == asset) return;

			_guid = asset->uuid.GetGuid();
			// https://jira.com2us.com/jira/browse/CSECOTS-3731
			LvAssetDatabase::SaveAsset(asset);

			for (const auto& each : asset->header.dependencies)
			{
				if (LvString::IsNullOrEmpty(each.value.uuid)) continue;

				Editor::LvAsset* refAsset = LvAssetDatabase::GetAssetByUuid(LvUuid::Parse(each.value.uuid));

				if (refAsset == nullptr) continue;
				if (refAsset->IsBuiltIn() || !refAsset->IsLoaded()) continue;

				const bool isSubAsset = refAsset->uuid.GetLocalId() != 0;

				if (isSubAsset)
				{
					refAsset = LvAssetDatabase::GetAssetByGuid(refAsset->uuid.GetGuid());
				}

				refAsset->Unload();
			}

			LvEditorScene* scene = static_cast<LvEditorScene*>(asset->GetPrototype());

			scene->Finalize();

			asset->Unload();
		}

		void Reload()
		{
			// NOTICE https://jira.com2us.com/jira/browse/CSECOTS-5888
			// Scene Reload시에 prefab이 가진 Scene Component의 포인터가 댕글링 날 수 있기 때문에 언로드 후 다시 로드해야한다.
			LvAsset* asset = LvAssetDatabase::GetAssetByGuid(_guid);
			if(asset != nullptr)
			{
				LV_LOG(debug, "ReloadScene");

				LvEditorScene* scene = static_cast<LvEditorScene*>(asset->GetData());

				scene->Initialize();

				LvEditorSceneManager::SetActive(scene);  // 작업할 scene으로 EditorContext에 할당
			}
		
		}

		void Clear()
		{
			//_stream.Dispose();
		}

	private:
		LvGuid _guid;

	};

	static ScriptChangeAction s_scriptChangeAction;
	
}

const char* s_buildType = CMAKE_INTDIR;

const char* LvProject::_createEditorEntryMethodName = "lv_create_editor_entry";
const char* LvProject::_destroyEditorEntryMethodName = "lv_destroy_editor_entry";

const char* LvProject::_createApplicationEntryMethodName = "lv_create_app_entry";
const char* LvProject::_destroyApplicationEntryMethodName = "lv_destroy_app_entry";

constexpr const char* s_entryBundleName = "EntryBundle";

static const LvTypeId s_assetType = LV_TYPEOF(LvAsset);
static const LvTypeId s_assetAttributeType = LV_TYPEOF(LvAssetAttribute);
static const LvTypeId s_versionAttributeType = LV_TYPEOF(LvSerializeVersionAttribute);

static LvManifestManagement s_manifestManagement;

// 경로에 맞춰 모든 폴더를 생성
bool directory_recursive_create(const char* path)
{
	if (nullptr == path) return false;

	static char separator = LV_DIRECTORY_SEPARATOR_CHAR;
	LvList<LvString> paths = LvString(path).Split(&separator);

#if defined(__WIN32__)
	LvString current = paths[0];
#else
	LvString current = LvString::Format("%s%s", &separator, paths[0].c_str());
#endif

	for (size_t i = 0; i < paths.Count(); i++)
	{
		if (!lv_directory_exist(current.c_str()))
		{
			if (!lv_directory_create(current.c_str())) // 여기서 실패하면 내부적인 문제로, 더 깊숙하게 생성할 수 없을것으로 판단하여 바로 탈출
			{
				return false;
			}
		}
		if (i + 1 < paths.Count())
		{
			current = lv_path_combine(current, paths[i + 1]);
		}
	}
	return true;
}

uint32 calcCrc32(const char* path)
{
	if (!lv_file_exist(path))
	{
		LV_LOG(warning, "Not found file (%s)", path);
		return 0;
	}

	LvFileStream stream = lv_file_open(path, LvFileAccess::READ_ONLY, LvFileMode::OPEN);
	void* m = lv_malloc(stream.Length());
	stream.ReadRaw(m, stream.Length());
	const uint32 crc = LvCRCHash::MemCrc32(m, (uint32)stream.Length());
	lv_free(m);

	return crc;
}

static std::function<LvShaderCache* (LvShader*, uint8/*subShaderIndex*/, uint8/*shaderPassIndex*/, Engine::Renderer::LvShaderPermutationHash)> cbBackup = Engine::LvShader::GetSafeShaderCache;
static std::function<LvShaderCache* (LvShader*, uint8/*subShaderIndex*/, uint8/*shaderPassIndex*/, Engine::Renderer::LvShaderPermutationHash)> cb =
[](LvShader* shader, uint8 subShaderIndex, uint8 shaderPassIndex, Engine::Renderer::LvShaderPermutationHash hash) -> LvShaderCache*
{
	if (shader == nullptr) return nullptr;

	LvShaderAsset* asset = static_cast<LvShaderAsset*>(LvAssetDatabase::GetAssetByInstanceId(shader->GetInstanceId()));
	if (asset == nullptr)
	{
		return shader->GetPass(shaderPassIndex)->GetShaderCache(hash);
	}

	return asset->GetCache(hash, subShaderIndex, shaderPassIndex);
};

enum class ActionType
{
	NONE = 0,
	REFRESH = 1 << 0,
	CLEAN_BUILD = 1 << 1,
};
static ActionType s_reserve = ActionType::NONE;

LvProject::LvProject(const char* projectPath, std::function<void()>&& _onAfterReleaseDllResource)
	: onPlayModeChangedEvent(&_playModeChangedEvent)
	, onBeforeLoadEvent(&_beforeLoadEvent)
	, onAfterLoadEvent(&_afterLoadEvent)
	, onBeforeUnloadEvent(&_beforeUnloadEvent)
	, onAfterUnloadEvent(&_afterUnloadEvent)
	, _projectAbsolutePath(projectPath)
	, _settingsFilePath(lv_path_combine(projectPath, lv_path_combine("Settings", "ProjectSettings.json")))
	, _libraryDirectoryPath(lv_path_combine(_projectAbsolutePath, "Solution"))
	, _temporaryLibraryFolderPath(getTemporaryLibraryDirectoryPath(projectPath))
	, _onAfterReleaseDllResource(std::move(_onAfterReleaseDllResource))
	, _libraryLoaders{&_applicableLoader, &_workspaceLoader }
	, _createMethodNames{ _createApplicationEntryMethodName, _createEditorEntryMethodName }                                                                                                                                                   
	, _destroyMethodNames{ _destroyApplicationEntryMethodName, _destroyEditorEntryMethodName }
	, _loopMutex(lv_mutex_create())
	, _loopCondition(lv_condition_create())
{
	if (LvProjectContext::s_isTest) return;

	lv_atomic_set(&_currentFramePreferenceSaveAtomic, 0);

	LvList<LvString> childFilePaths;

#if defined(__MACOSX__)
	lv_file_find(_libraryDirectoryPath.c_str(), "*.xcodeproj", childFilePaths, 0);
#else 
	lv_file_find(_libraryDirectoryPath.c_str(), "*.sln", childFilePaths, 0);
#endif
	
	_solutionFileName = lv_path_name_without_extension(projectPath);

	if (!childFilePaths.IsEmpty())
	{
		_solutionFileName = lv_path_name_without_extension(childFilePaths[0].c_str());
	}

	//const LvString projectName = lv_path_name_without_extension(absolutePath);

	_libraryNames[static_cast<uint8>(LibraryType::APPLICATION)].FormatSelf("%s%s", _solutionFileName.c_str(), "-Native");
	_libraryNames[static_cast<uint8>(LibraryType::EDITOR)].FormatSelf("%s%s", _solutionFileName.c_str(), "-NativeEditor");

	if (!lv_file_exist(_settingsFilePath.c_str()))
	{
		SaveSettings();
	}
	else
	{
		LoadSettings();
	}

	if (lv_directory_exist(_temporaryLibraryFolderPath.c_str()))
	{
		if (!lv_directory_remove(_temporaryLibraryFolderPath.c_str()))
		{
			// 임시 폴더를 지우지 못한 상태
			LV_LOG(warning, "Failed remove %s", _temporaryLibraryFolderPath.c_str());
		}
	}

	LvProjectPreferences::onValueWasUpdated.AddListener(&LvProject::onValueWasUpdated, this);
	LvFileManager::onDetectEvent.AddListener(&LvProject::onDetectFileManager, this);

	Engine::LvShader::GetSafeShaderCache = cb;
        
	LvDispatchQueue("Project Loop").Async([&]()
		{
			lv_atomic_set(&_loopAtomic, 1);
			// @donghun 백그라운드에서 60fps 기준 매 프레임 변경내역 감시
			while (true)
			{
				lv_thread_sleep(16);

				auto action = [&]()
				{
					LvDispatchQueue::Main().Sync([&]()
						{
							if (!lv_editor_get_context()->isSleepMode && PlayState::STOP == GetPlayState())
							{								
								const bool sameLibrary = _lastChangeLibraryCrc32[static_cast<uint8>(LibraryType::APPLICATION)] == calcCrc32(_copiedLibraryPaths[static_cast<uint8>(LibraryType::APPLICATION)].c_str()) &&
														_lastChangeLibraryCrc32[static_cast<uint8>(LibraryType::EDITOR)] == calcCrc32(_copiedLibraryPaths[static_cast<uint8>(LibraryType::EDITOR)].c_str());

								if (LvFlagUtil::Contains(s_reserve, ActionType::CLEAN_BUILD))
								{
									// @donghun 파일들이 변경된게 감지 되었지만 라이브러리 파일은 수정된게 없다면 빌드를 재시도
									if (sameLibrary)
									{
										LvProjectGenerator::AsyncCleanBuild(!lv_editor_get_context()->settings.backgroundBuild);
									}
								}
								else if (LvFlagUtil::Contains(s_reserve, ActionType::REFRESH))
								{
									if (LvProjectGenerator::Update())
									{
										LvProjectGenerator::Generate();
									}
								}

								s_reserve = ActionType::NONE;
							}
						});
				};

				if (ActionType::NONE != s_reserve && !lv_editor_get_context()->isSleepMode)
				{
					action();
				}

				if (0 == lv_atomic_get(&_loopAtomic)) break;		
			}
			lv_condition_notify_one(_loopCondition);
		});
}

LvProject::~LvProject()
{
	if (1 == lv_atomic_get(&_loopAtomic))
	{
		lv_atomic_set(&_loopAtomic, 0);
		if (false == LvProjectContext::s_isTest)
		{
			lv_condition_wait(_loopCondition, _loopMutex, LV_WAIT_INFINITE);	
		}
	}

	LvFileManager::onDetectEvent.RemoveListener(&LvProject::onDetectFileManager, this);
	LvProjectPreferences::onValueWasUpdated.RemoveListener(&LvProject::onValueWasUpdated, this);

	unloadLibraries();

	lv_condition_destroy(_loopCondition);
	lv_mutex_destroy(_loopMutex);

	Engine::LvShader::GetSafeShaderCache = cbBackup;
}

void LvProject::InitializeVersionController()
{
	LV_PROFILE_EDITOR();
    LvProjectContext* project = lv_project_get_context();
    LvProjectSettings& settings = project->settings;
    const LvEngineContext* engine = lv_engine_get_context();

    const uint64 engineVersion[4] = { engine->version.major, engine->version.minor, engine->version.patch, engine->version.build };
    const uint64 projectVersion[4] = { settings.major, settings.minor, settings.patch, settings.build };

    LvVersionController::Initialize(engineVersion, projectVersion, engine->system->executableDirectoryPath, _projectAbsolutePath.c_str());
}

/**
 * @brief 유저DLL의 빌드가 끝났을때를 감지하고 모두 해당 리소스를 로드합니다.
 *
 * 유저DLL이 빌드되는중에 dll파일이 계속해서 변하기때문에, 최종적으로 변할때까지 기다리다가
 * 최종적으로 수정된  dll파일을 로드해야합니다. 이를 해결하기 위해 유저DLL 빌드가 끝날때 chk
 * 파일을 생성하는데, 이 파일의 변화를 감지하여 dll을 로드합니다.
 * 하지만 mac에서는 이것이 통하지 않습니다. chk파일이 갱신되고 나서 dylib가 들어오기때문에 강제딜레이를 주어야 합니다.
 */
// (Project::ChangeType evtype, const char* src, const char* dst)
void LvProject::onDetectFileManager(const LvFileManagerEventArgs& args)
{
	if (lv_path_contains(LvFileManager::ToAbsolutePath(LV_PROGRAM_DIR).c_str(), args.absoluteSourcePath))
	{
		onChangeSourceCode(args);
	}

	if (lv_path_extension(args.absoluteSourcePath) != ".chk")
	{
		return;
	}

	// const LvString& fileName = lv_path_name_without_extension(args.absoluteSourcePath);

	/*
	@brief 프로젝트 리로트 하는 로직을 Sync하게 수정
	 - 수정 이유 : 간헐적 유저 프로젝트 저장시 TypeMissing 이 발생하는 현상 존재
	   저장중 프로젝트를 리로드하여 Reflection을 unRegist한 타이밍에
	   Scene을 제작하거나 Deserialize 하여 TypeMissing이 발생하는것으로 추정
	*/
	if (lv_current_main_thread())
	{
		ReloadLibrary();
		// for (size_t i = 0; i < static_cast<size_t>(LibraryType::MAX); ++i)
		// {
		// 	if (fileName == _libraryNames[i])
		// 	{
		// 		reloadLibrary(static_cast<LibraryType>(i));
		// 	}
		// }
	}
	else
	{
		LvDispatchQueue::Main().Sync([this/*, fileName*/] 
		{
			ReloadLibrary();
			// for (size_t i = 0; i < static_cast<size_t>(LibraryType::MAX); ++i)
			// {
			// 	if (fileName == _libraryNames[i])
			// 	{
			// 		reloadLibrary(static_cast<LibraryType>(i));
			// 	}
			// }
		});
	}
}

void LvProject::onChangeSourceCode(const Project::LvFileManagerEventArgs& args)
{
	LvString fileName = lv_path_name(args.absoluteSourcePath);
	fileName.ToUpper();
	const bool isCMakeLists = fileName == "CMAKELISTS.TXT";
	// CMakeLists.txt가 변경될땐 리프레시가 될 필요가 없음
	if (isCMakeLists)
	{
		s_reserve = LvFlagUtil::Add(s_reserve, ActionType::REFRESH);
	}

	// 빌드가 시작되면 gen.cpp가 변경되기 때문에 idle인 상태에서만 시도
	if (LvProjectGenerator::buildState.IsIdle() && lv_editor_get_context()->settings.autoBuild)
	{
		// CMakeLists.txt가 변경될땐 자동 빌드 하지 않음		
		if (!isCMakeLists)
		{
			LV_LOG(debug, "Detect change file (%s)", args.absoluteSourcePath);
			s_reserve = LvFlagUtil::Add(s_reserve, ActionType::CLEAN_BUILD);
		}
		_lastChangeLibraryCrc32[static_cast<uint8>(LibraryType::APPLICATION)] = calcCrc32(_copiedLibraryPaths[static_cast<uint8>(LibraryType::APPLICATION)].c_str());
		_lastChangeLibraryCrc32[static_cast<uint8>(LibraryType::EDITOR)] = calcCrc32(_copiedLibraryPaths[static_cast<uint8>(LibraryType::EDITOR)].c_str());
	}
}

void LvProject::onValueWasUpdated(const char* key)
{
	_valueWasUpdatedCount++;

	if (lv_editor_get_context()->settings.preferencesValueWasUpdatedMaxCount <= _valueWasUpdatedCount)
	{
		if (0 == lv_atomic_get(&_currentFramePreferenceSaveAtomic))
		{
			lv_atomic_set(&_currentFramePreferenceSaveAtomic, 1);
			LvDispatchQueue::Main().Async([&]()
				{
					_valueWasUpdatedCount = 0;
					LvProjectPreferences::Save();
					lv_atomic_set(&_currentFramePreferenceSaveAtomic, 0);
				});
		}
	}
}

void LvProject::changeState(const PlayState state)
{
	const PlayState beforeState = _playState;
	_playState = state;
	_playModeChangedEvent->Dispatch(PlayModeEventArgs(beforeState, _playState));
}

bool LvProject::loadLibraries()
{
	if (lv_current_main_thread())
	{
		loadLibrary(LibraryType::APPLICATION);
		loadLibrary(LibraryType::EDITOR);

		LvString path;
		path.Append(lv_path_parent(_copiedLibraryPaths[static_cast<uint8>(LibraryType::APPLICATION)].c_str()));
		path.Append(';');
		path.Append(lv_path_parent(_copiedLibraryPaths[static_cast<uint8>(LibraryType::EDITOR)].c_str()));

		Diagnostics::LvCallStack::Initalize(path);

		return _isLoadedLibraries[static_cast<uint8>(LibraryType::APPLICATION)] && _isLoadedLibraries[static_cast<uint8>(LibraryType::EDITOR)];
	}
	else
	{
		bool result = false;
		LvDispatchQueue::Main().Sync([&] {
			loadLibrary(LibraryType::APPLICATION);
			loadLibrary(LibraryType::EDITOR);

			LvString path;
			path.Append(lv_path_parent(_copiedLibraryPaths[static_cast<uint8>(LibraryType::APPLICATION)].c_str()));
			path.Append(';');
			path.Append(lv_path_parent(_copiedLibraryPaths[static_cast<uint8>(LibraryType::EDITOR)].c_str()));

			Diagnostics::LvCallStack::Initalize(path);

			result = _isLoadedLibraries[static_cast<uint8>(LibraryType::APPLICATION)] && _isLoadedLibraries[static_cast<uint8>(LibraryType::EDITOR)];
			});
		return result;
	}
}

void LvProject::unloadLibraries()
{
	// @donghun Editor > Application 순서로 내려야 함
	unloadLibrary(LibraryType::EDITOR);
	unloadLibrary(LibraryType::APPLICATION);
}

bool LvProject::ReloadLibrary()
{
	LV_CHECK_MAIN_THREAD();
	bool result = true;

	unloadLibraries();

	result &= loadLibrary(LibraryType::APPLICATION);
	result &= loadLibrary(LibraryType::EDITOR);

	return result;
}

bool LvProject::loadLibrary(LibraryType type)
{
	LV_CHECK_MAIN_THREAD();
	const uint8 index = static_cast<uint8>(type);
	if (_libraryLoaders[index]->IsLoadedHandle())
	{
		return false;
	}

	_copiedLibraryPaths[index] = copyToTemp(type);
	if (_copiedLibraryPaths[index].IsEmpty())
	{
		_beforeLoadEvent->Dispatch(type);

		if (LibraryType::APPLICATION == type) s_scriptChangeAction.Reload();
		if (LibraryType::EDITOR == type) _workspace = createWorkspace();

		_afterLoadEvent->Dispatch(type, false);

		LV_LOG(warning, "Failed to copy library.");
		return false;
	}
	else
	{
		_beforeLoadEvent->Dispatch(type);
		const char* create = _createMethodNames[index];
		const char* destory = _destroyMethodNames[index];

		_isLoadedLibraries[index] = _libraryLoaders[index]->LoadHandle(_copiedLibraryPaths[index].c_str(), create, destory);

		if (LibraryType::APPLICATION == type) s_scriptChangeAction.Reload();
		if (LibraryType::EDITOR == type) _workspace = createWorkspace();

		_afterLoadEvent->Dispatch(type, _isLoadedLibraries[index]);
		return _isLoadedLibraries[index];
	}
}

void LvProject::releaseLibrary(LibraryType type)
{
	switch (type)
	{
	case LibraryType::APPLICATION:
	{
		_onAfterReleaseDllResource();
	}
	break;
	case LibraryType::EDITOR:
	{
		if (_workspace) destroyWorkspace(_workspace);
	}
	break;

	default: break;
	}
}

void LvProject::unloadLibrary(LibraryType type)
{
	LV_CHECK_MAIN_THREAD();

	LvManagedJobSystem::UnregistAll(_libraryNames[static_cast<int>(type)].c_str());
	const uint8 index = static_cast<uint8>(type);
	
	if (LibraryType::APPLICATION == type)
	{
		if (PlayState::STOP == _playState)
		{
			s_scriptChangeAction.Prepare();
		}
	}

	// @notice: s_scriptChangeAction.Prepare();에서 씬 해제 시 리플렉션을 이용하기 때문에 여기서 리플렉션 언레지스트 수행.
	LvReflection::UnregistAll(_libraryNames[static_cast<int>(type)].c_str());

	if (!_isLoadedLibraries[index]) return;

	_beforeUnloadEvent->Dispatch(type);

	releaseLibrary(type);
	_libraryLoaders[index]->UnloadHandle();
	_lastChangeLibraryCrc32[index] = 0;
	_isLoadedLibraries[index] = false;
	_afterUnloadEvent->Dispatch(type);
}

bool check_version(const LvAssetType assetType,  LvBuiltInResource::Info& info, const LvString& absPath, const bool compile, const bool import)
{
	// for only shader
	if(assetType != LvAssetType::SHADER)
	{
		return false;
	}
	
	const LvSystemContext* systemContext = lv_system_get_context();
	const LvString& configPath = lv_path_combine(systemContext->executableDirectoryPath, LvEngineContext::Directories::BuiltIn::resource);

	LvString assetTypeName = LvEnum::GetName<LvAssetType>(assetType);
	assetTypeName.ToLower();

	const LvString& versionName = LvBuiltInResource::MakeIdentifyName(info.path, info.version.latest);
	const LvString& targetPath = lv_path_combine(configPath, { assetTypeName.c_str(), versionName.c_str() });
	const LvString orgFile = lv_path_combine(configPath, { assetTypeName.c_str(), info.path.c_str() });

	// TODO: 추후에 없어져야 할 부분, 프로젝트 하나에서 여러 버전을 동시에 로드할 수 있다...
	bool isChanged = false;
	if(import == false)
	{
		LvString infPath = targetPath;
		infPath.Append(".inf");

		char checksum[33];
		lv_md4_file(orgFile.c_str(), checksum);
		LvString md4(checksum);

		LvTextAssetInfo assetInfo;
		if (assetInfo.Load(infPath.c_str()))
		{
			if (assetInfo.testStr != md4)
			{
				isChanged = true;

				assetInfo.testStr = md4;
				assetInfo.Save(infPath.c_str());
			}
		}
		else
		{
			assetInfo.testStr = md4;
			assetInfo.Save(infPath);
		}

		info.recompile = !lv_file_exist(targetPath.c_str()) || compile || isChanged;	// for LV_DEVELOPE_RENDER, 전체 재컴파일이 필요한 경우가 있다.	
	}

	// versioin 붙은 이름으로 working directory에 복사
	//lv_file_copy(targetPath.c_str(), orgFile.c_str());
	lv_file_copy_with_attribute(targetPath.c_str(), orgFile.c_str());

	const LvString lastestPath = lv_path_combine(LvEditorContext::Directories::builtinresource, { assetTypeName.c_str(), versionName.c_str() });
	const LvString& lastestVersionPath = lv_path_combine(absPath, lastestPath);
	const bool versionUp = lv_file_exist(lastestVersionPath.c_str()) == false;
	const bool recompile = isChanged || versionUp || compile;

	lv_file_copy_with_attribute(lastestVersionPath.c_str(), targetPath.c_str());

	if (recompile || import)
	{
		for (uint32 ii = 1; ii < info.version.latest; ++ii)
		{
			const LvString lowVersionName = LvBuiltInResource::MakeIdentifyName(info.path, ii);
			const LvString relativePath = lv_path_combine(LvEditorContext::Directories::builtinresource, { assetTypeName.c_str(), lowVersionName.c_str() });
			const LvString lowVersionPath = lv_path_combine(absPath, relativePath);
			
			if (lv_file_exist(lowVersionPath.c_str()))
			{
				if(recompile)
				{
					lv_file_copy(lowVersionPath.c_str(), targetPath.c_str());
				}

				if(import)
				{
					// 여기서 import TODO : 추후 없어져야 할 부분!
					if (LvAssetDatabase::ImportAsset(relativePath, true))
					{
						const LvString entryBundleName = LvBundleManifestManager::GetEntryBundleName();
						LvBundleManifest* manifest = LvBundleManifestManager::GetManifest(entryBundleName);
						LvAsset* asset = LvAssetDatabase::GetAssetByPath(relativePath);
						LvBundleDependencyController::AddAsset(entryBundleName, asset);
					}
				}
			}
		}
	}
	
	return true;
}

bool preloadHLSL(const LvString& userProjectPath)
{
	// hlsl asset이 shader asset 보다 먼저 임포트 돼있어야 한다.
	const LvSystemContext* systemContext = lv_system_get_context();
	const LvString& configPath = lv_path_combine(systemContext->executableDirectoryPath, lv_path_combine("builtin", "resource"));

	// E:\Work\Project\build\Debug\BuiltIn\Resource\shader
	const LvString& dirPath = lv_path_combine(configPath, "shader");

	const LvString targetBuiltInPath = lv_path_combine(userProjectPath.c_str(), LvEditorContext::Directories::builtinresource);
	const LvString targetBuiltInShaderPath = lv_path_combine(targetBuiltInPath.c_str(), "shader");
	
	lv_directory_copy(targetBuiltInShaderPath.c_str(), dirPath.c_str(), L"\\*.hlsl.inf");

	lv_directory_copy(targetBuiltInShaderPath.c_str(), dirPath.c_str(), L"\\*.hlsl");


	//LvList<LvString> relPaths;
	LvStack<LvString> stack;
	stack.Push(dirPath);
	//relPaths.Reserve(64);

	while (!stack.IsEmpty())
	{
		LvString dir = stack.Pop();
		LvList<LvString> list = lv_directory_lists(dir.c_str(), true, true);

		LvString dirWithSep = userProjectPath;
		dirWithSep.Append(LV_DIRECTORY_SEPARATOR_CHAR);
		const uint32 total = (uint32)list.Count();
		for (uint32 i = 0; i < total; ++i)
		{
			auto& file = list[i];

			const LvString currentPath = lv_path_combine(dir, file);
			
			// E:\Work\Project\build\Debug\builtin\resource\shader\com2us
			if (lv_directory_exist(currentPath.c_str()))
			{

				LvString projectFilePath = LvString::Replace(currentPath, configPath.c_str(), "");
				projectFilePath = lv_path_combine(targetBuiltInPath.c_str(), projectFilePath.c_str());
				// abs : E:\Projects\Project_37 + builtin/resource

				//info도 모두 복사
				lv_directory_copy(projectFilePath.c_str(), currentPath.c_str(), L"\\*.hlsl.inf");

				lv_directory_copy(projectFilePath.c_str(), currentPath.c_str(), L"\\*.hlsl");
				stack.Push(currentPath);
			}
			else
			{
				const LvString ext = lv_path_extension_to_lower(currentPath.c_str());
				if( ext == ".hlsl")
				{
					LvString buildPath = systemContext->executableDirectoryPath;
					buildPath.Append(LV_DIRECTORY_SEPARATOR_CHAR);

					LvString relPath = LvString::Replace(currentPath, configPath.c_str(), "");
					relPath = lv_path_combine(LvEditorContext::Directories::builtinresource, relPath.c_str());
					
					if (LvAssetDatabase::ImportAsset(relPath, true))	// 앞에 builtin 부분 대소문자 구분 함
					{
						LvAsset* asset = LvAssetDatabase::GetAssetByPath(relPath);
						LV_ASSERT(asset->type == LvAssetType::HLSL, "check it!");
						{
							LvHLSLAsset::AddChangedHLSL(static_cast<LvHLSLAsset*>(asset));
						}
					}
				}
			}
		}
	}

	return true;
}


void LvProject::LoadBuiltIn(LvString& state) const
{
	LV_PROFILE_EDITOR();

	LvBuiltInUtil::SetResourceMode();
	LvBuiltInUtil::SetUserProjectPath(GetAbsolutePath());

	preloadHLSL(GetAbsolutePath());

	const LvSystemContext* systemContext = lv_system_get_context();
	const LvProjectContext* projectContext = lv_project_get_context();

	const LvString& configPath = lv_path_combine(systemContext->executableDirectoryPath, LvEngineContext::Directories::BuiltIn::resource);

	size_t i = 0;
	size_t total = 0;

	for (auto& assetType : LvBuiltInResource::Configuration)
	{
		total += assetType.value.Count();
	}

	// install path
	const char* installResName = LvEngineContext::Directories::BuiltIn::resource;
	const LvString& dirPath = projectContext->builtinDirectoryPath;

	for (auto& assetType : LvBuiltInResource::Configuration)
	{
		LvString assetTypeName = LvEnum::GetName<LvAssetType>(assetType.key);
		assetTypeName.ToLower();

		for(size_t j = 0, max = assetType.value.Count(); j < max; ++j)
		{
			auto& file = assetType.value[j];
			const float progress = static_cast<float>(i++) / total;
			state.FormatSelf("Load Built In %s ... (%.1f%%)", file.name.c_str(), progress * 100.f);

			// 셰이더는 실시간 컴파일 필요
			const LvString& versionName = LvBuiltInResource::MakeIdentifyName(file.path, file.version.latest);

			const LvString& assetTypePath = lv_path_combine(dirPath, assetTypeName);
			const LvString& relativePath = lv_path_combine(LvEditorContext::Directories::builtinresource, { assetTypeName.c_str(), versionName.c_str() });
			const LvString& targetPath = lv_path_combine(configPath, { assetTypeName.c_str(), versionName.c_str() });

			//LV_CHECK(lv_file_exist(targetPath.c_str()), "You might be missed to build the install target");
			const LvString& dstPath = lv_path_combine(GetAbsolutePath(), relativePath);
			
			const LvString directory = lv_path_parent(dstPath.c_str());
			if (!lv_directory_exist(directory.c_str()))
			{
				lv_directory_create(directory.c_str());
			}

			const LvString& targetInfoPath = targetPath + ".inf";
			const LvString& dstInfoPath = dstPath + ".inf";

			if (!lv_file_exist(targetInfoPath.c_str()))//install 후 build/builtin 에 info가 없는 resource가 있는경우 에러
			{
				LV_LOG(error, "YOU MUST MAKE BUILTIN INFO FILE %s", targetInfoPath.c_str());
			}

			if (!lv_file_exist(dstInfoPath.c_str()))
			{
				lv_file_copy(dstInfoPath.c_str(), targetInfoPath.c_str());
			}

			check_version(assetType.key, file, GetAbsolutePath(), false, false);

			if (!lv_file_exist(dstPath.c_str()))
			{
				lv_file_copy(dstPath.c_str(), targetPath.c_str());
			}
		}
	}
	
	LvList<LvString> relPaths;
	LvStack<LvString> stack;
	stack.Push(dirPath);

	while (!stack.IsEmpty())
	{
		LvString dir = stack.Pop();
		LvList<LvString> list = lv_directory_lists(dir.c_str(), true, true);

		LvString dirWithSep = _projectAbsolutePath;
		dirWithSep.Append(LV_DIRECTORY_SEPARATOR_CHAR);

		for (i = 0, total = list.Count(); i < total; ++i)
		{
			auto& file = list[i];

			LvString absPath = lv_path_combine(dir, file);
			LvString relPath = LvString::Replace(absPath, dirWithSep.c_str(), "");

			if (lv_directory_exist(absPath.c_str()))
			{
				stack.Push(absPath);
			}
			else
			{
				relPaths.Add(relPath);
			}
		}
	}
	for(i = 0, total = relPaths.Count(); i < total; ++i)
	{
		const LvString& relPath = relPaths[i];
		LvString file = lv_path_name(relPath.c_str());
		const LvString dir = lv_path_parent(relPaths[i].c_str());

		LvString numbers;
		LvString name;

		LvString dirPathWithSep = dirPath;
		dirPathWithSep.Append(LV_DIRECTORY_SEPARATOR_CHAR);
		
		const char* begin = file.c_str();
		const char* lookahead = begin;
		
		while (*lookahead != '\0')
		{
			if (*(lookahead - 1) == '_' && *lookahead == 'v' && LvLexer::IsNumber(*(lookahead + 1)))
			{
				name = LvString(begin, (lookahead - 1) - begin);
				lookahead++;
		
				while (LvLexer::IsNumber(*lookahead))
				{
					numbers.Append(*lookahead);
					lookahead++;
				}
				break;
			}
			lookahead++;
		}

		bool deprecated = false;
		const LvAssetType type = LvAssetExtension::GetAssetTypeByExtension(lv_path_extension(relPath.c_str()));
		if (!numbers.IsEmpty() && LvAssetType::NOT_ASSET != type)
		{
			uint32 version = LvStringUtil::ToUint32(numbers.c_str());

			const auto& list = LvBuiltInResource::Configuration[type];

			for (const auto& info : list)
			{
				if (info.name.StartsWith(name))
				{
					if (version <= info.version.deprecated)
					{
						deprecated = true;
						LV_LOG(warning, "%s builtin file is deprecated", relPath.c_str());
					}
				}
			}
		}

		if (!deprecated)
		{
			if (type == LvAssetType::HLSL)	// 따로 먼저 처리합니다.
			{
				continue;
			}

			const float progress = static_cast<float>(i) / total;
			state.FormatSelf("Load Built In %s ... (%.1f%%)", file.c_str(), progress * 100.f);

			if (LvAssetDatabase::ImportAsset(relPath, true))
			{
			}
		}
	}

	// built-in shader cache를 미리 만들어둬야 한다.
	PrecompileShaders(state);
}

void LvProject::SaveShaderCacheKey()
{
	// Get the current time 
	time_t curTime = time(NULL);

	// Convert the current time 
	struct tm* pLocal = NULL;
#if defined(_WIN32) || defined(_WIN64) 
	pLocal = localtime(&curTime);
#else 
	localtime_r(&curTime, pLocal);
#endif 
	if (pLocal == NULL)
	{
		// Failed to convert the current time 
		return;
	}

	const LvString key = LvString::Format("%04d%02d%02d_%02d%02d%02d", pLocal->tm_year + 1900, pLocal->tm_mon + 1, pLocal->tm_mday,
		pLocal->tm_hour, pLocal->tm_min, pLocal->tm_sec);

	const LvEngineContext* engine = lv_engine_get_context();
	LvProjectSettings& settings = lv_project_get_context()->settings;
	settings.shaderCacheKey = key;

	// 자동으로 세이브 하도록.
	SaveSettings();

	LV_LOG(debug, "[%s]ProjectSettings is saved. It will remove cache and compile all shaders", key.c_str());
}

bool LvProject::PrecompileShaders(LvString& state) const
{	
	const LvString dirPath = LvBuiltInUtil::GetEngineBuiltInShaderPath(); // executableDirectoryPath + /builtin/resource/shader
	const LvString cachePath = lv_path_combine(dirPath.c_str(), "cache");
	// cache 폴더를 미리 만들어 둔다.
	{
#ifdef MAKE_BUILT_IN_SHADER_CACHE
		 lv_directory_remove(cachePath.c_str());		
#endif
		if (!lv_directory_exist(cachePath.c_str()))
		{
			lv_directory_create(cachePath.c_str());
		}
	}

	// 위에서 import만 하고 캐시를 안 만들어놔서 여기서 만든다.
	const uint32 count = static_cast<uint32>(LvBuiltInResource::Configuration[LvAssetType::SHADER].Count());
	for (uint32 ii = 0; ii < count; ++ii)
	{
		const float progress = static_cast<float>(ii) / count;

		LvShaderAsset* shaderAsset = static_cast<LvShaderAsset*>(LvBuiltInResource::Get(LvAssetType::SHADER, static_cast<LvBuiltInResource::ShaderPreset>(ii)));
		LV_ASSERT(shaderAsset != nullptr, "shader asset is nullptr");
		state.FormatSelf("Compiling Shader %s ... (%.1f%%)", shaderAsset->name.c_str(), progress * 100.f);

		LvShader* shader = static_cast<LvShader*>(shaderAsset->GetData());
		LV_ASSERT(shader != nullptr, "shader is nullptr");

		for (uint32 passIndex = 0; passIndex < shader->GetSubShader(0)->GetPassCount(); ++passIndex)
		{
			Engine::Renderer::LvShaderPermutationHash hash = shader->GetPass(passIndex)->GetDefaultHash();
			const LvShaderCache* cache = shaderAsset->GetCache(hash, 0, passIndex);
			LV_ASSERT(cache != nullptr, "GetCache is failed!");
		}

		// /executableDirectoryPath + builtin/resource/shader 쪽에 최신 빌트인 셰이더 캐시를 tod
		{
#ifdef MAKE_BUILT_IN_SHADER_CACHE
			LvAssetDatabase::SaveAsset(shaderAsset);
			
			const LvString userProjectCachePath = shaderAsset->GetAbsoluteCachePath();
			const LvString cacheName = lv_path_name(userProjectCachePath.c_str());
			const LvString targetPath = lv_path_combine(cachePath.c_str(), cacheName.c_str());

			lv_file_copy(targetPath.c_str(), userProjectCachePath.c_str());
#endif
		}
	}

	return true;
}


void LvProject::CompileAdditionalShaders()
{
	const LvList<LvString> shaders = LvHLSLAsset::GetChangedShaderList();
	for (const auto& shaderRelPath : shaders)
	{
		if (LvAssetDatabase::ImportAsset(shaderRelPath))
		{
			LvAsset* asset = LvAssetDatabase::GetAssetByPath(shaderRelPath);
			LV_ASSERT(asset != nullptr, "check it!");
		}
		else
		{
			LV_LOG(debug, "failed import %s", shaderRelPath.c_str());
		}
		// LvFileManager::Reimport(shaderAbsPath.c_str()); -> 내부에서 builtin에 대한 처리를 별도로 또 해줘야 한다... 어차피 import asset으로 수렴하므로 그냥 바로 호출
	}
}

void LvProject::LoadSettings() const
{
	LvFileStream settingsFile(_settingsFilePath.c_str(), LvFileMode::OPEN);
	LvInputStream inputStream(&settingsFile);

	const LvString settingJsonString = inputStream.ReadToEnd();

	settingsFile.Flush();
	settingsFile.Close();

	LvProjectContext* project = lv_project_get_context();

	LvProjectSettings& settings = project->settings;
	
	LvJsonDomArchive archive(settingJsonString.c_str());
	archive.Deserialize(LvReflection::GetTypeId<LvProjectSettings>(), &settings);

	LvEngineContext* engineContext = lv_engine_get_context();
	engineContext->sceneGraphPath = settings.defaultSceneGraphFileName;

	if (LvString::IsNullOrEmpty(settings.windowLayoutFilePath))
	{
		settings.windowLayoutFilePath = LvPanelController::defaultDockingLayoutPath;
	}

	engineContext->isGPUskinning = settings.isGPUskinning;
	if(settings.useHDR)
	{
		engineContext->colorRenderTextureFormat = Render::LvPixelFormat::R16G16B16A16_SFLOAT;
	}
	else
	{
		engineContext->colorRenderTextureFormat = Render::LvPixelFormat::R8G8B8A8_UNORM;
	}
	engineContext->debugWireFrame = settings.debugWireFrame;
	engineContext->debugVertexNormal = settings.debugVertexNormal;
	engineContext->debugFaceNormal = settings.debugFaceNormal;
	engineContext->debugBone = settings.debugBone;

	if (!settings.shaderCacheKey.IsEmpty())
	{
		// library에 있는 캐시 파일이 있으면 비교 후 다를 경우 + 캐시 파일이 없을 경우 -> 캐시 모두 제거
		const LvString libraryPath = lv_path_combine(GetAbsolutePath(), LvEditorContext::Directories::library);
		
		//const LvString shaderCompileInfoName = LvString::Format("ShaderCompile_%llu", settings.allCompileShaderTime);
		const LvString shaderCacheKeyName = LvString::Format("ShaderCompile_%s", settings.shaderCacheKey.c_str());
		const LvString shaderCacheKeyPath = lv_path_combine(libraryPath, shaderCacheKeyName);
		
		bool found = false;
		// 파일이 컴파일마다 여러 개 생성되지 않도록 나머지는 지워준다.
		LvList<LvString> fileList = lv_directory_lists(libraryPath.c_str(), true, false);	// 폴더 제외하고 파일만
		for (const auto& filename : fileList)
		{
			if (filename == shaderCacheKeyName)
			{
				// 있다는 것 자체가 컴파일 할 필요가 없다는 것.
				found = true;
				continue;
			}
			else if (filename.SubString(0, LvString("ShaderCompile_").Length()) == "ShaderCompile_")
			{
				const LvString rmShader = lv_path_combine(libraryPath, filename);
				lv_file_delete(rmShader.c_str());
			}
		}

		if (!found)
		{
			if (!lv_directory_exist(libraryPath.c_str()))
			{
				lv_directory_create(libraryPath.c_str());
			}

			// 파일을 만든다.
			lv_file_create(shaderCacheKeyPath.c_str());

			lv_directory_remove(lv_path_combine(libraryPath, LvEditorContext::Directories::Cache::builtInShader).c_str());	// built-in shader
			lv_directory_remove(lv_path_combine(libraryPath, LvEditorContext::Directories::Cache::shader).c_str());			// shader
			lv_directory_remove(lv_path_combine(libraryPath, LvEditorContext::Directories::Cache::asset).c_str());			// asset cache
		}
	}
}

void LvProject::MigrateVersion()
{
	LV_PROFILE_EDITOR();
	LvProjectContext* project = lv_project_get_context();
	LvProjectSettings& settings = project->settings;
	const LvEngineContext* engine = lv_engine_get_context();

	const LvString projectPath = lv_path_parent(project->resourcesDirectoryPath.c_str());

#ifdef _DEBUG
	const uint64 engineVersion[4] = { engine->version.major, engine->version.minor, engine->version.patch, engine->version.build };
	const uint64 projectVersion[4] = { settings.major, settings.minor, settings.patch, settings.build };
#endif

	if (LvVersionController::MigrateProject())
	{
		_migrated = true;
	}

	LoadSettings();

	settings.major = engine->version.major;
	settings.minor = engine->version.minor;
	settings.patch = engine->version.patch;
	settings.build = engine->version.build;

	SaveSettings();
}

void LvProject::ShowMigrationResult()
{
	if (_migrated) LvVersionController::ShowResult();
}

void LvProject::CheckAssets()
{
	LV_PROFILE_EDITOR();
	// 이 메서드에서 버전 정보가 다른 리소스를 현재 버전으로 마이그레이션
	struct Target
	{
		uint32 version;
		LvTypeId prototype;
	};

	LvHashtable<LvTypeId, Target> manageds;

	const auto info = LvReflection::GetTypeInfo(s_assetType);

	// Asset 을 상속받는 모든 클래스에서 AssetAttribute 에서 Prototype 을 가져와야하고
	// Prototype 에 명시된 Version 을 가져와야한다.

	for (auto each : info->subTypes)
	{
		const auto subInfo = LvReflection::GetTypeInfo(each);

		for (const auto& attrInfo : subInfo->attributes)
		{
			if (attrInfo.type == s_assetAttributeType)
			{
				const auto attr = static_cast<LvAssetAttribute*>(attrInfo.attribute);

				const auto protoInfo = LvReflection::GetTypeInfo(attr->prototype);

				for (const auto& protoAttrInfo : protoInfo->attributes)
				{
					if (protoAttrInfo.type == s_versionAttributeType)
					{
						if (!manageds.ContainsKey(each))
						{
							const LvSerializeVersionAttribute* verAttr = static_cast<LvSerializeVersionAttribute*>(protoAttrInfo.attribute);
							Target target { 0, 0};
							target.version = verAttr->version;
							target.prototype = attr->prototype;

							manageds.Add(each, target);
							break;
						}
					}
				}
			}
		}
	}

	// 등록된 모든 Asset 인스턴스에서 type 에 등록된 버전과 파일에 버전을 체크한다.
	for(const auto& pair : LvAssetDatabaseInternal::GetGuidAssetRelation())
	{
		const auto asset = pair.value;

		auto type = asset->GetType();
		if (!manageds.ContainsKey(type)) continue;
		if (asset->IsBuiltIn()) continue;
		if (asset->GetResourceFormat() == LvAsset::ResourceFormat::BINARY || asset->GetResourceFormat() == LvAsset::ResourceFormat::TEXT) continue;

		const Target& target = manageds[type];
		
		if (asset->header.serializeVersion == -1)
		{
			asset->header.serializeVersion = static_cast<int32>(target.version);
			continue;
		}

		const int32 targetVersion = static_cast<int32>(target.version);
		if (targetVersion != asset->header.serializeVersion)
		{
			// 스레드 경쟁 상태에 놓일 수 있음
			LV_LOG(warning, "If this happens, the ProjectLoad thread and the importing job thread may be in a race condition.");

			LvVersionController::MigrateResources(target.prototype, asset->path.c_str(), asset->header.serializeVersion);

			LvAssetDatabase::ImportAsset(lv_path_combine(LvEngineContext::Directories::resources, asset->path).c_str());

			_migrated = true;

			const int32 newVersion = asset->header.serializeVersion;
			if (targetVersion != newVersion)
			{
				LV_THROW("Unmatched asset version, you should make the version migrator %s", asset->path.c_str());
			}
		}

		if (!asset->HasCache())
		{
			LV_LOG(debug, "%s asset cache load fails, so re-record the cache", asset->path.c_str());

			LvAssetDatabaseInternal::RecordCache(asset); 
		}
	}

}

void LvProject::SaveSettings()
{
	const LvEngineContext* engine = lv_engine_get_context();
	LvProjectSettings& settings = lv_project_get_context()->settings;

	if (LvString::IsNullOrEmpty(settings.windowLayoutFilePath))
	{
		settings.windowLayoutFilePath = LvPanelController::defaultDockingLayoutPath;
	}

	LvString settingJsonString;
	LvJsonDomArchive archive;
	
	const LvString directory = lv_path_directory(_settingsFilePath.c_str());
	if (!lv_directory_exist(directory.c_str()))
	{
		lv_directory_create(directory.c_str());
	}

	LvFileStream settingsFile(_settingsFilePath.c_str(), LvFileMode::CREATE);
	LvOutputStream outputStream(&settingsFile);

	settings.appleDevelopmentTeamId = lv_editor_get_context()->settings.appleDevelopmentTeamId;
	
	settings.major = engine->version.major;
	settings.minor = engine->version.minor;
	settings.patch = engine->version.patch;
	settings.build = engine->version.build;

	archive.Serialize(LvReflection::GetTypeId<LvProjectSettings>(), &settings);
	settingJsonString = archive.GetResult();
	outputStream.WriteText(settingJsonString.c_str());

	settingsFile.Flush();
	settingsFile.Close();

}

Lv::Engine::LvApplicable * LvProject::createApplicable()
{
	return _applicableLoader.Create(lv_engine_get_context());
}

void LvProject::destoryApplicable(Engine::LvApplicable* applicable)
{
	_applicable->Destroy();
	_applicableLoader.Destroy(applicable);
	_applicable = nullptr;
}

LvWorkspace* LvProject::createWorkspace()
{
	return _workspaceLoader.Create(lv_editor_get_context());
}

void LvProject::destroyWorkspace(LvWorkspace* workspace)
{
	_workspace->Finalize();
	_workspaceLoader.Destroy(_workspace);
	_workspace = nullptr;
}

LvString LvProject::getLibraryDirectoryPath(const char * projectPath) const
{
	return getLibraryDirectoryPath_ForBuildType(projectPath, s_buildType);
}

LvString LvProject::getTemporaryLibraryDirectoryPath(const char* projectPath) const
{
	return getTemporaryLibraryDirectoryPath_ForBuildType(projectPath, s_buildType);
}

LvString LvProject::getLibraryDirectoryPath_ForBuildType(const char * projectPath, const char* buildType) const
{
#if defined(__WIN32__)
	return lv_path_combine(projectPath, { "Solution", buildType, buildType });
#else
	return lv_path_combine(projectPath, { "Solution", buildType,"lib",buildType });
#endif
}

LvString LvProject::getTemporaryLibraryDirectoryPath_ForBuildType(const char* projectPath, const char* buildType) const
{
#if defined(__WIN32__)
	return lv_path_combine(projectPath, { "Solution", "Temp", buildType, buildType });
#else
	return lv_path_combine(projectPath, { "Solution", "Temp", buildType, "lib", buildType });
#endif
}

LvString LvProject::copyToTemp(LibraryType type) const
{
	LvString tailFileName = "";
	LvString projectName = LvString::Empty();
	const LvString cmakeListPath = lv_path_combine(_projectAbsolutePath, "CMakeLists.txt");

	static const char* ProjectValue = "__LV_VALUE__PROJECT_NAME";
	static const char* EditorValue = "__LV_VALUE__EDITOR_PROJECT_NAME";

	const char* SetValue = nullptr;

	switch (type)
	{
	case LibraryType::APPLICATION:

		tailFileName = "-Native"; // Project-Native.dll, libProject-Native.dylib
		SetValue = ProjectValue;
		break;
	case LibraryType::EDITOR:
		tailFileName = "-NativeEditor"; // Project-NativeEditor.dll, libProject-NativeEditor.dylib
		SetValue = EditorValue;
		break;
	default:
		break;
	}

	if (lv_file_exist(cmakeListPath.c_str()))
	{
		LvFileStream stream(cmakeListPath.c_str(), LvFileMode::OPEN);
		LvInputStream is(&stream);
		LvString cmakeList = is.ReadToEnd();

		const int32 idx = cmakeList.IndexOf(SetValue);
		int32 begin = -1;
		for (int32 i = idx; i < cmakeList.Length(); ++i)
		{
			if (cmakeList[i] == '\"')
			{
				if (begin == -1)
				{
					begin = i + 1;
				}
				else
				{
					projectName = cmakeList.SubString(begin, i - begin);
					tailFileName = LvString::Empty();
					break;
				}
			}
		}
		stream.Close();
	}
	else
	{
		const int lastSeparator = lv_str_last_index_of(_projectAbsolutePath.c_str(), LV_DIRECTORY_SEPARATOR_CHAR);
		if (lastSeparator == -1)
		{
			projectName = _projectAbsolutePath.c_str();
		}
		else
		{
			projectName = _projectAbsolutePath.c_str() + lastSeparator + 1;
		}
	}

	LvString dllFileName;
#if defined(__WIN32__)
	dllFileName.Append(projectName);
	dllFileName.Append(tailFileName);
	dllFileName.Append(".dll");
#else
	dllFileName.Append("lib");
	dllFileName.Append(projectName);
	dllFileName.Append(tailFileName);
	dllFileName.Append(".dylib");
#endif
	const LvString dllSourcePath = lv_path_combine(_libraryDirectoryPath.c_str(), dllFileName.c_str()); // TODO

	if (!lv_file_exist(dllSourcePath.c_str()))
	{
		LV_LOG(warning, "Not found file (%s)", dllSourcePath.c_str());
		return "";
	}

	size_t failed = 0;
	LvString dllDestinationFile;

	// 복사가 성공할때 까지 시도
	while (true)
	{
		LvString tempDllFileName;
#if defined(__WIN32__)
		tempDllFileName.Append(projectName);
		tempDllFileName.Append(tailFileName);
		if (0 < failed) tempDllFileName.AppendFormat("-%zu", failed);
		tempDllFileName.Append(".dll");
#else
		tempDllFileName.Append("lib");
		tempDllFileName.Append(projectName);
		tempDllFileName.Append(tailFileName);
		if (0 < failed) tempDllFileName.AppendFormat("-%zu", failed);
		tempDllFileName.Append(".dylib");
#endif

		dllDestinationFile = lv_path_combine(_temporaryLibraryFolderPath, tempDllFileName.c_str());

		if (!lv_directory_exist(_temporaryLibraryFolderPath.c_str()))
		{
			lv_directory_create(_temporaryLibraryFolderPath.c_str());
		}
		if (!lv_file_copy(dllDestinationFile.c_str(), dllSourcePath.c_str()))
		{
			// 라이브러리를 임시영역으로 복사하지 못하면 사용하지 못할 수 있음
			// dll이 빌드되지 않은 경우 이 경고가 출력될 수 있음
			LV_LOG(warning, "Warning project dll copy failed (%s)", dllDestinationFile.c_str());
			++failed;
		}
		else
		{
			LV_LOG(info, "Copy from %s to %s successful", dllSourcePath.c_str(), dllDestinationFile.c_str());
			break;
		}
	}

	if (lv_file_exist(dllDestinationFile.c_str()))
	{
		return dllDestinationFile;
	}
	else
	{
		LV_LOG(warning, "Not found file (%s)", dllDestinationFile.c_str());
		return "";
	}
}

void LvProject::Play()
{
	if (PlayState::PLAY == _playState)
	{
		LV_LOG(warning, "Already played.");
		return ;
	}

	LvProjectContext* context = lv_project_get_context();
	
	if (PlayState::STOP == _playState)
	{
		if (_playMode == PlayMode::BUNDLE)
		{
			// bundle play로 실행하기 위해 옮겨야 할 것
			// User Project의 dll를 copy해서 옮기는 임시 폴더로 옮겨서 실행한다.
			// 해당 Folder에 BundlePlay를 만들고, 다음과 같은 파일들이 있을 것이다.
			//  - BundlePlay/Play-Entry.exe
			//  - BundlePlay/engine.dll
			//	- BundlePlay/Bundles...
			//	- BundlePlay/BuildSetting File
			//	- BundlePlay/Resources/SceneGraph File
			//	- BundlePlay/libGLESv2.dll or libGLESv2.dylib (win32 or mac)
			//	- BundlePlay/libEGL.dll or libEGL.dylib (win32 or mac)
			// 해당 위치의 폴더가 없으면 먼저 폴더를 생성한다.
			static const char* bundlePlayTempFolderName = "BundlePlay";
			const LvString& tempDllFolderPath = _temporaryLibraryFolderPath;
			LvFixedString<LV_CHAR_INIT_LENGTH> bundlePlayFolderPath(tempDllFolderPath.c_str());
			bundlePlayFolderPath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
			bundlePlayFolderPath.Append(bundlePlayTempFolderName);

			LvFixedString<LV_CHAR_INIT_LENGTH> bundlePlayResourceFolderPath(bundlePlayFolderPath);
			bundlePlayResourceFolderPath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
			bundlePlayResourceFolderPath.Append(LvEngineContext::Directories::resources);

			if (false == lv_directory_exist(bundlePlayFolderPath.c_str()))
			{
				lv_directory_create(bundlePlayFolderPath.c_str());
			}

			if (false == lv_directory_exist(bundlePlayResourceFolderPath.c_str()))
			{
				lv_directory_create(bundlePlayResourceFolderPath.c_str());
			}

			LvEngineContext* engContext = lv_engine_get_context();
			LvSystemContext* sysContext = lv_system_get_context();

			LvFixedString<LV_CHAR_INIT_LENGTH> playEntryExePath;
			playEntryExePath.Append(sysContext->executableDirectoryPath);
			playEntryExePath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
#if defined(__WIN32__)
			playEntryExePath.Append("Play-Entry.exe");
#elif defined(__MACOSX__)
			playEntryExePath.Append("Play-Entry");
#endif
			LvList<LvFixedString<LV_CHAR_INIT_LENGTH>> sourceFiles;
			sourceFiles.Add(playEntryExePath);

			// engine.dll
			{
				LvFixedString<LV_CHAR_INIT_LENGTH> engineDllPath;
				engineDllPath.Append(sysContext->executableDirectoryPath);
				engineDllPath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
#if defined(__WIN32__)
				engineDllPath.Append("engine.dll");
#elif defined(__MACOSX__)
				engineDllPath.Append("libengine.dylib");
#endif

				sourceFiles.Add(engineDllPath);
			}

			LvEngineContext* engineContext = lv_engine_get_context();
			// BundlePlay/libGLESv2.dll or libGLESv2.dylib (win32 or mac)
			// BundlePlay/libEGL.dll or libEGL.dylib (win32 or mac)
			if (engineContext->renderPlatform == Render::LvRenderPlatform::GLES3)
			{
				LvFixedString<LV_CHAR_INIT_LENGTH> glesDllPath;
				glesDllPath.Append(sysContext->executableDirectoryPath);
				glesDllPath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
#if defined(__WIN32__)
				glesDllPath.Append("libGLESv2.dll");
#elif defined(__MACOSX__)
				glesDllPath.Append("libGLESv2.dylib");
#endif
				sourceFiles.Add(glesDllPath);

				LvFixedString<LV_CHAR_INIT_LENGTH> eglDllPath;
				eglDllPath.Append(sysContext->executableDirectoryPath);
				eglDllPath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
#if defined(__WIN32__)
				eglDllPath.Append("libEGL.dll");
#elif defined(__MACOSX__)
				eglDllPath.Append("libEGL.dylib");
#endif
				sourceFiles.Add(eglDllPath);
			}

			// NOTICE BundlePlay에서 번들이 필수이기 때문에 Manifest에 해당하는 번들이 존재하는지 확인 후 없으면 Play를하지 않는다.
			for(const auto& each : LvBundleManifestManager::GetManifests())
			{
				LvString bundlePath = LvBundleManifestManager::GetBundleFullPath(each->GetBundleName(), LvPlatformType::STANDALONE);
				if(!lv_file_exist(bundlePath.c_str()))
				{
					LvMessagePanel* message = lv_editor_get_window()->CreatePanel<LvMessagePanel>();
					message->Open("Play Fail", "Not all bundles were built.");
					return;
				}
			}

			LvString platformDir = lv_path_combine(engContext->bundleDirectoryPath.c_str(), lv_get_platform_type_to_string(LvPlatformType::STANDALONE));

			// lv_directory_lists는 full path가 아닌 상대경로를 준다.
			LvList<LvString> bundleFolderFileLists = lv_directory_lists(platformDir.c_str(), true, false);
			for (size_t bundleFileIndex = 0; bundleFileIndex < bundleFolderFileLists.Count(); ++bundleFileIndex)
			{
				const LvString& bundleFolderFileName = bundleFolderFileLists[bundleFileIndex];

				if (true == lv_path_check_extension(bundleFolderFileName.c_str(), LvAssetBundle::BundleExtension))
				{
					LvFixedString<LV_CHAR_INIT_LENGTH> fullBundlePath;
					fullBundlePath.Append(engContext->bundleDirectoryPath);
					fullBundlePath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
					fullBundlePath.Append(lv_get_platform_type_to_string(LvPlatformType::STANDALONE));
					fullBundlePath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
					fullBundlePath.Append(bundleFolderFileName);

					sourceFiles.Add(fullBundlePath);
				}
			}

			// 먼저 BundlePlay 폴더로 옮겨준다.
			for (size_t sourceFileIndex = 0; sourceFileIndex < sourceFiles.Count(); ++sourceFileIndex)
			{
				const LvFixedString<LV_CHAR_INIT_LENGTH>& sourceFilePath = sourceFiles[sourceFileIndex];
				int sourceFileLastSeparator = lv_str_last_index_of(sourceFilePath.c_str(), LV_DIRECTORY_SEPARATOR_CHAR);

				LvFixedString<LV_CHAR_INIT_LENGTH> targetFilePath;
				targetFilePath.Append(bundlePlayFolderPath);
				targetFilePath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
				if (sourceFileLastSeparator == -1)
				{
					targetFilePath.Append(sourceFilePath);
				}
				else
				{
					targetFilePath.Append(sourceFilePath.c_str() + sourceFileLastSeparator + 1);
				}

				lv_file_copy(targetFilePath.c_str(), sourceFilePath.c_str());
			}

			LvProjectSettings& settings = lv_project_get_context()->settings;

			// Build Settings File
			{
				LvBuildSettings buildSettings;
				LvProjectSettings projectSetting = settings;

				buildSettings.sceneGraphFileName = projectSetting.defaultSceneGraphFileName;
				buildSettings.GPUskinningSetting = projectSetting.isGPUskinning;

				buildSettings.debugWireFrame = projectSetting.debugWireFrame;
				buildSettings.debugVertexNormal = projectSetting.debugVertexNormal;
				buildSettings.debugFaceNormal = projectSetting.debugFaceNormal;
				buildSettings.debugBone = projectSetting.debugBone;
				buildSettings.fixedAspect = projectSetting.fixedAspect;
				buildSettings.aspect = projectSetting.fixedAspect ? projectSetting.aspect : projectSetting.resolution;

				LvFixedString<LV_CHAR_INIT_LENGTH> buildSettingFilePath;
				buildSettingFilePath.Append(bundlePlayFolderPath);
				buildSettingFilePath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
				buildSettingFilePath.Append(LV_BUILD_SETTING_FILE_NAME);

#define BUILD_SETTING_JSON 1
#if BUILD_SETTING_JSON
				LvJsonDomArchive jsonArchive;
				buildSettings.Serialize(jsonArchive);

				LvString json = jsonArchive.GetResult();

				LvFileStream fs(buildSettingFilePath.c_str(), LvFileMode::CREATE);
				fs.WriteRaw(json.c_str(), json.Length());
#else
				LvMemoryStream<> stream;
				LvOutputStream ostream(&stream);
				LvBinaryArchive binArchive(ostream);

				buildSettings.Serialize(binArchive);

				LvFileStream fs(buildSettingFilePath.c_str(), LvFileMode::CREATE);
				fs.WriteRaw(stream.GetMemory(), stream.GetPosition());
				fs.Flush();
				fs.Close();
#endif
				fs.Close();
			}

			// Scene Graph 파일을 BundlePlay/Resources 폴더로 옮겨준다.
			{
				LvFixedString<LV_CHAR_INIT_LENGTH> sourceSceneGraphPath;
				sourceSceneGraphPath.Append(engContext->resourcesDirectoryPath);
				sourceSceneGraphPath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
				sourceSceneGraphPath.Append(settings.defaultSceneGraphFileName);

				LvFixedString<LV_CHAR_INIT_LENGTH> targetSceneGraphPath;
				targetSceneGraphPath.Append(bundlePlayResourceFolderPath);
				targetSceneGraphPath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
				targetSceneGraphPath.Append(settings.defaultSceneGraphFileName);

				LvString targetDirectory = lv_path_directory(targetSceneGraphPath.c_str());
				if(!lv_directory_exist(targetDirectory.c_str()))
				{
					lv_directory_create(targetDirectory.c_str());
				}

				lv_file_copy(targetSceneGraphPath.c_str(), sourceSceneGraphPath.c_str());
			}

			// UserSetting 파일도 BundlePlay/Resources 폴더로 옮겨준다.
			{
				LvFixedString<LV_CHAR_INIT_LENGTH> userSettingPath;
				userSettingPath.Append(engContext->resourcesDirectoryPath);
				userSettingPath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
				userSettingPath.Append(LvUserSetting::userSettingFileName);

				LvFixedString<LV_CHAR_INIT_LENGTH> targetUserSettingPath;
				targetUserSettingPath.Append(bundlePlayResourceFolderPath);
				targetUserSettingPath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
				targetUserSettingPath.Append(LvUserSetting::userSettingFileName);
				lv_file_copy(targetUserSettingPath.c_str(), userSettingPath.c_str());
			}
			// execute new process 
			playEntryExePath.Clear();
			playEntryExePath.Append(bundlePlayFolderPath);
			playEntryExePath.Append(LV_DIRECTORY_SEPARATOR_CHAR);
#if defined(__WIN32__)
			playEntryExePath.Append("Play-Entry.exe");
#elif defined(__MACOSX__)
			playEntryExePath.Append("Play-Entry");
#endif
			LvList<const char*> cmdArgs;

#if defined(__WIN32__)			
			cmdArgs.Add("Windows");
#elif defined(__MACOSX__)
			cmdArgs.Add("Mac");
#endif
			cmdArgs.Add(sysContext->graphicLibrary.name);
			cmdArgs.Add(_copiedLibraryPaths[static_cast<uint8>(LibraryType::APPLICATION)].c_str());

			LvSystemContext* systemContext = lv_system_get_context();
			// destribution
			{
				const LvString& builtinDstDir = lv_path_combine(bundlePlayFolderPath, LvEngineContext::Directories::BuiltIn::distribute);

				if (!lv_directory_exist(builtinDstDir.c_str()))
				{
					const LvString& builtinSrcDir = lv_path_combine(systemContext->executableDirectoryPath, LvEngineContext::Directories::BuiltIn::distribute);

					lv_directory_create(builtinDstDir.c_str());

					for (auto type : Engine::LvBuiltInDistribute::Configuration)
					{
						LvString typeName = LvEnum::GetName(type);
						typeName.ToLower();

						const LvString& typeDir = lv_path_combine(builtinDstDir, typeName.c_str());
						if (!lv_directory_exist(typeDir.c_str()))
						{
							lv_directory_create(typeDir.c_str());
						}

						for (auto info : Engine::LvBuiltInDistribute::Configuration[type])
						{
							LvString name = Engine::LvBuiltInDistribute::MakeIdentifyPath(type, info.name, info.version.latest);
							LvString path = lv_path_combine(typeName, name);

							LvString srcPath = lv_path_combine(builtinSrcDir, path);
							LvString dstPath = lv_path_combine(builtinDstDir, path);

							lv_file_copy(dstPath.c_str(), srcPath.c_str());
						}
					}

					LvString srcPath = lv_path_combine(builtinSrcDir, Engine::LvBuiltInDistribute::ConfigFileName);
					LvString dstPath = lv_path_combine(builtinDstDir, Engine::LvBuiltInDistribute::ConfigFileName);

					lv_file_copy(dstPath.c_str(), srcPath.c_str());
				}
			}

			// resource
			{
				const LvString& builtinSrcDir = lv_path_combine(systemContext->executableDirectoryPath, LvEngineContext::Directories::BuiltIn::resource);
				const LvString& builtinDstDir = lv_path_combine(bundlePlayFolderPath, LvEngineContext::Directories::BuiltIn::resource);

				if (!lv_directory_exist(builtinDstDir.c_str()))
				{
					lv_directory_create(builtinDstDir.c_str());

					LvString srcPath = lv_path_combine(builtinSrcDir, LvBuiltInResource::ConfigFileName);
					LvString dstPath = lv_path_combine(builtinDstDir, LvBuiltInResource::ConfigFileName);

					lv_file_copy(dstPath.c_str(), srcPath.c_str());

				}
			}
			//lv_platform_process_create(playEntryExePath.c_str(), (char*)commandLineArgument.c_str());
			//const char* args[1] = { commandLineArgument.c_str() };

			lv_process_create(playEntryExePath.c_str(), cmdArgs.data(), cmdArgs.Count());
		}
		else
		{
			if (!LvEditorSceneGraph::CheckAllSceneContainsBundle())
			{
				LV_LOG(warning, "Some of Scene doesn't have main bundle");
				return;
			}

			if (nullptr == _applicable)
			{
				_applicable = createApplicable();
				if (nullptr == _applicable)
				{
					if(lv_editor_get_context()->settings.disableLoadLibrary)
					{
						LV_LOG(error, "The library loading disable option is set, so the library was not loaded. (HOW TO FIX : Edit/Editor Settings/Disable Library load option OFF)");
					}
					else
					{
						LV_LOG(error, "Failed to load application library (HOW TO FIX : File/Project Build and File/Project Reload)");
					}
					return;
				}
			}

			//LvBundleDatabase::RegistLoader(&LvEditorBundleLoader::GetInstance());

			LvBundleRepository::SetManagement(&s_manifestManagement);

			const auto& initname = LvSceneGraphInternal::GetInitName();

			LvEditorSceneGraph* graph = LvEngineAllocator::New<LvEditorSceneGraph>("Runtime-SceneGraph");

			_beforeEditingScene = LvEditorSceneManager::GetActive();
			_beforeSceneGraph = LvSceneGraphInternal::GetInstance();

			LvSceneGraphInternal::SetInstance(graph);
			LvSceneGraphInternal::SetInitName(initname);
			// 에디터 씬그래프 초기화. _applicable->Init때 엔트리 노드 다음 씬이 초기화 되기 때문에 그 전에 호출 필요!
			{
				LvEditorSceneGraph::Initialize();
			}

			changeState(PlayState::PLAY);

			_applicable->Load();
			_applicable->Init();

			//TODO: AnimationComponent과 PropertyPath를개선하면 삭제
			LvPropertyPathHelper::SetClearIndexAll();

			LvScene* scene = LvSceneGraph::GetCurrent();

			if (scene == nullptr)
			{
				LV_LOG(warning, "Application can't play because the current scene of SceneGraph is empty");
				Stop();
			}

			if (scene->name != _beforeEditingScene->name)
			{
				LvSelection::activeEntities.Clear(true, false);
			}
		}
	}
	else
	{
		changeState(PlayState::PLAY);
	}	
}

bool LvProject::CanOpen(const char* absolutePath)
{
	if (!lv_directory_exist(absolutePath))
	{
		LV_LOG(warning, "Not found path");
		return false;
	}

	if (!lv_file_exist(lv_path_combine(absolutePath, "CMakeLists.txt").c_str()))
	{
		LV_LOG(warning, "Not found CMakeLists");
		return false;
	}

	return true;
}

void LvProject::Open(const char* projectAbsolutePath)
{
	if (!LvProject::CanOpen(projectAbsolutePath))
	{
		LvMessagePanel* message = lv_editor_get_window()->CreatePanel<LvMessagePanel>();
		message->Open("Fail", "Could not load a project.");
		return;
	}

	LvString path = projectAbsolutePath;
	LvDispatchQueue("Load Project").Async([path]() {
		LvCommand::Commit<LvProjectLoad>(path.c_str());
		});
}

void LvProject::Pause()
{
	if (PlayState::PAUSE == _playState)
	{
		LV_LOG(warning, "Already paused.");
		return;
	}
	
	LvProjectContext* context = lv_project_get_context();
	changeState(PlayState::PAUSE);
	_shouldPauseNextFrame = false;
};

void LvProject::Stop()
{
	if (PlayState::STOP == _playState)
	{
		LV_LOG(warning, "Already stoped.");
		return;
	}

	const PlayMode mode = _playMode;

	if (mode == PlayMode::EDITOR)
	{
		if (PlayState::STOP == _playState)
		{
			LV_CHECK(_applicable == nullptr, "Something is wrong.");
			return;
		}

		if (nullptr != _applicable)
		{
			LV_CHECK(_beforeSceneGraph != nullptr, "Something is wrong.");
			LV_CHECK(_beforeEditingScene != nullptr, "Something is wrong.");

			// 제거하기 전에 activeScene nullptr로 비워주기
			LvEditorSceneManager::SetActive(nullptr);

			LvScene* afterScene = LvSceneGraph::GetCurrent();
			LvSceneGraphInternal* afterGraph = LvSceneGraphInternal::GetInstance();

			if (afterScene != nullptr)
			{
				afterScene->Finalize();

				// 에디터 씬 그래프 정리 에디터에서Stop시 꼭 호출 해주어야 함!
				LvEditorSceneGraph::Finalize();
			}
	
			destoryApplicable(_applicable);

			// _applicable에서 Finalize를 호출해주고 거기서 임시 노드그래프의 노드들 정리함
			//LvSceneGraphInternal::Finalize();

			LvEngineAllocator::Free(afterGraph);

			LvSceneGraphInternal::SetInstance(_beforeSceneGraph);
			LvEditorSceneManager::SetActive(_beforeEditingScene);

			// TODO 일반화된 해법이 필요합니다. https://jira.com2us.com/jira/browse/CSECOTS-6616 이슈처리용
			LvAssetDatabaseInternal::RemoveObjectsLoadedAtPlay();
		}

		//TODO: AnimationComponent과 PropertyPath를개선하면 삭제
		LvPropertyPathHelper::SetClearIndexAll();

		// AppEntry에서 LvSceneGraph::Finalize를 하면서 모든 Scene을 Renderer에 대해 Unregist하므로
		// 여기에서 다시 Regist를 해준다.
		//Engine::Renderer::LvRenderPath* renderer = engineContext->render->GetRenderer();
		//LV_CHECK(renderer->HasRegistScene(LvSelection::activeScene) == false, "This Scene Should not be Registered yet now");	// 로직 확인 차 검사
		//renderer->RegistScene(LvSelection::activeScene);
	}
	else if (mode == PlayMode::BUNDLE)
	{

	}

	changeState(PlayState::STOP);
}

void LvProject::NextFrame()
{
	_shouldPauseNextFrame = true;
	if (PlayState::PLAY != _playState) Play();
}

void LvProject::Close()
{
	s_scriptChangeAction.Clear();
	LvCommand::Execute<LvProjectClose>();

	LV_LOG(debug, "[CLOSE] Clear command");
	LvCommand::Clear();
}

// Applicatoin, Editor 모두 copy 되었다면, 둘다 isEmpty 가 false 다.
bool LvProject::CheckLibrariesCopied() const
{
	return _copiedLibraryPaths[static_cast<uint8>(LibraryType::APPLICATION)].IsEmpty() == false &&
		_copiedLibraryPaths[static_cast<uint8>(LibraryType::EDITOR)].IsEmpty() == false;
}

bool LvProject::IsLoaded() { return nullptr != lv_project_get_context(); }

LvManifestManagement& LvProject::GetManifestManagement() { return s_manifestManagement; }
LV_NS_EDITOR_END
