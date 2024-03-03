#include "LvPrecompiled.h"
#include "editor/project/LvProjectGenerator.h"

#include "system/LvProcess.h"
#include "system/LvFileSystem.h"
#include "system/LvSystemContext.h"

#include "engine/thirdparty/imgui/imgui.h"
#include "engine/thread/LvDispatchQueue.h"

#include "editor/project/LvProject.h"
#include "editor/window/LvEditorWindow.h"
#include "editor/coreclr/LvCoreclrBridge.h"

#include "editor/LvProgress.h"
#include "editor/panel/source_browser/LvSimpleCMakeAST.h"

using namespace Lv;
using namespace Engine;

LV_NS_EDITOR_BEGIN

LvProjectGenerator::BuildState LvProjectGenerator::buildState;

bool LvProjectGenerator::Update(bool removeAndUpdate)
{
	const auto context = lv_project_get_context();
	if (nullptr != context)
	{
		return Update(context->project->GetAbsolutePath(), removeAndUpdate);
	}
	LV_LOG(warning, "Project not open");
	return false;
}

bool LvProjectGenerator::Update(const char* projectPath, bool removeAndUpdate)
{
	if (!buildState.IsIdle())
	{
		// @donghun 빌드 중일때는 CMake Generate를 해서는 안됨
		LV_LOG(warning, "Cannot be processed during build");
		return false;
	}

	return cmakeListsUpdate(projectPath, removeAndUpdate);
}

bool LvProjectGenerator::Generate()
{
	const auto context = lv_project_get_context();
	if (nullptr != context)
	{
		return Generate(context->project->GetAbsolutePath());
	}
	LV_LOG(warning, "Project not open");
	return false;
}

bool LvProjectGenerator::Generate(const char* projectPath)
{
	if (!buildState.IsIdle())
	{
		// @donghun 빌드 중일때는 CMake Generate를 해서는 안됨
		LV_LOG(warning, "Cannot be processed during build");
		return false;
	}
	
	if (0 != cmakeGenerate(projectPath))
	{
		LV_LOG(error, "CMake Configuring incomplete (HOW TO FIX : Remove the 'app/CMakeLists.txt' or 'editor/CMakeLists.txt' and try again)");		
	}

	return true;
}

void LvProjectGenerator::AsyncCleanBuild(const bool showProgress)
{
	const auto context = lv_project_get_context();
	if (context)
	{
		AsyncCleanBuild(context->project->GetAbsolutePath(), showProgress);
	}
	else
	{
		LV_LOG(warning, "Project not open");
	}
}

void LvProjectGenerator::AsyncCleanBuild(const char* projectPath, const bool showProgress)
{
	static LvString s_projectPath;
	s_projectPath = projectPath;

	LvDispatchQueue("Build user project").Async([showProgress] {
		bool r = CleanBuild(s_projectPath.c_str(), showProgress);
	});	
}

void LvProjectGenerator::CmakeListsUpdate(const LvList<LvString>& parameters)
{
	if (parameters.Count())
	{
		cmakeListsUpdate(parameters[0].c_str());
	}
	exit(0);
}

bool LvProjectGenerator::IsVaildCMakeVersion(const char* projectPath)
{	
	return CoreCLR::ProjectManager::IsValidCMakeVersion(projectPath);
}

void LvProjectGenerator::MigrateVersion(const char* projectPath)
{
	LV_PROFILE_EDITOR();
	CoreCLR::ProjectManager::DeleteCMakeLists(projectPath);
	CoreCLR::ProjectManager::CreateCMakeLists(projectPath);
	cmakeListsUpdate(projectPath);
}

void LvProjectGenerator::CreateProject(const char* projectPath)
{
	const LvSystemContext* context = lv_system_get_context();	
		
	// CoreCLR::ProjectManager::CreateProjectDirectories(projectPath);

	CoreCLR::ProjectManager::CreateEntryCodes(context->executableDirectoryPath, projectPath);

	CoreCLR::ProjectManager::CreateCMakeLists(projectPath);
	
	lv_editor_get_window()->ClearProgressBar();
}

bool LvProjectGenerator::Clean()
{
	const auto context = lv_project_get_context();
	if (context)
	{
		return Clean(context->project->GetAbsolutePath());
	}

	LV_LOG(warning, "Project not open");
	return false;
}

bool LvProjectGenerator::Clean(const char* projectPath)
{
	if (!buildState.IsIdle())
	{
		// @donghun 빌드 중일때는 CMake 관련 파일을 수정해서는 안됨
		LV_LOG(warning, "Cannot be processed during build");
		return false;
	}

	cmakeClean(projectPath);

	return true;
}

bool LvProjectGenerator::cmakeListsUpdate(const char* projectPath, bool removeAndUpdate)
{
	static LvSimpleCMakeAST ast;
	auto update = [](const char* cmakelistPath, const char* absolutePath, const LvList<LvString>& ignores, const char* setName) {
		ast.Configure(absolutePath, ignores, cmakelistPath, setName);
		LvString cmakeLists;
		if (ast.Generate(cmakelistPath, &cmakeLists))
		{
			LvFileStream stream(cmakelistPath, LvFileMode::CREATE);
			LvOutputStream outputStream(&stream);
			outputStream.WriteText(cmakeLists.c_str());
			stream.Flush();
			stream.Close();
			LV_LOG(warning, "Update CMakeLists (%s)", cmakelistPath);
		}
		else
		{
			LV_LOG(warning, "The CMakeLists has not been updated.");
		}
	};

	const LvString sourcePath = lv_path_combine(projectPath, LV_PROGRAM_NATIVE_SRC_DIR);
	const LvString applicationPath = lv_path_combine(projectPath, LV_PROGRAM_NATIVE_SRC_APP_DIR);
	const LvString editorPath = lv_path_combine(projectPath, LV_PROGRAM_NATIVE_SRC_EDITOR_DIR);

	const LvString projectCmakePath = lv_path_combine(projectPath, "CMakeLists.txt");
	const LvString applicatioCmakePath = lv_path_combine(applicationPath, "CMakeLists.txt");
	const LvString editorCmakePath = lv_path_combine(editorPath, "CMakeLists.txt");
	const LvList<LvString> ignorePaths =
	{
		lv_path_combine(applicationPath, "private/reflect"),
		lv_path_combine(applicationPath, "log"),
		lv_path_combine(editorPath, "private/reflect"),
		lv_path_combine(editorPath, "log"),
	};

	if(removeAndUpdate)
	{
		if (lv_file_exist(projectCmakePath.c_str()))
		{
			lv_file_delete(projectCmakePath.c_str());
		}
		if (lv_file_exist(applicatioCmakePath.c_str()))
		{
			lv_file_delete(applicatioCmakePath.c_str());
		}
		if (lv_file_exist(editorCmakePath.c_str()))
		{
			lv_file_delete(editorCmakePath.c_str());
		}
	}

	if(!lv_file_exist(applicatioCmakePath.c_str()) || !lv_file_exist(editorCmakePath.c_str()))
	{
		CoreCLR::ProjectManager::CreateCMakeLists(projectPath);
	}

	update(applicatioCmakePath.c_str(), applicationPath.c_str(), ignorePaths, "APPLICATION");
	update(editorCmakePath.c_str(), editorPath.c_str(), ignorePaths, "EDITOR");

	return true;
}

void LvProjectGenerator::cmakeClean(const char* projectPath)
{
	// OS 전환 시 cmake --build PATH --target clean 는 제대로 동작하지 않기 때문에 해당 캐시 데이터를 직접 제거
	// https://stackoverflow.com/questions/9680420/looking-for-a-cmake-clean-command-to-clear-up-cmake-output

	const char* cmakeFiles = "CMakeFiles";
	const char* cmakeCache = "CMakeCache.txt";

	const LvString mainCmakeFilesAbsolutePath = lv_path_combine(projectPath, { LV_SOLUTION_DIR, cmakeFiles });
	const LvString cmakeCacheAbsolutePath = lv_path_combine(projectPath, { LV_SOLUTION_DIR, cmakeCache });

	if (lv_file_exist(cmakeCacheAbsolutePath.c_str()))
	{
		lv_file_delete(cmakeCacheAbsolutePath.c_str());
	}

	if (lv_directory_exist(mainCmakeFilesAbsolutePath.c_str()))
	{
		lv_directory_remove(mainCmakeFilesAbsolutePath.c_str());
	}	
}

int LvProjectGenerator::cmakeGenerate(const char* projectPath)
{
#if !defined(LV_LEAK_CHECK)
	LvString args = LvString::Format("-DLV_ENGINE_BIN=\"%s\"", lv_path_current());
#else
	LvString args = LvString::Format("-DLV_LEAK_CHECK=ON -DLV_ENGINE_BIN=\"%s\"", lv_path_current());
#endif

	return CoreCLR::ProjectManager::GenerateProjectCmake(projectPath, args.c_str());
}

bool LvProjectGenerator::cmakeCleanAndUpdateAndGenerate(const char* projectPath)
{
	bool onCancel = false;
	bool onCancelResult = true;
	const int32 id = LvProgress::Start("User project", nullptr, ProgressOptions::INDEFINITE);

	LvProgress::RegisterCancelCallback(id, [&]
		{
			onCancel = true;
			return onCancelResult;
		});

	do
	{
		LvProgress::Report(id, "CMake Clean...");
		cmakeClean(projectPath);
		if (onCancel) break;

		LvProgress::Report(id, "CMake Generate...");
		if(!cmakeListsUpdate(projectPath))
		{
			onCancel = true;
		}
		cmakeGenerate(projectPath);

		if (onCancel) break;

	} while (false);

	lv_editor_get_window()->ClearProgressBar();

	onCancelResult = false;
	if (onCancel)
	{
		LvProgress::Finish(id, Status::CANCELED);
		buildState.cancel();

		// @donghun 빌드 중 취소되었다면 뒤에 있는 빌드를 진행하지 않는다.
		return false;
	}
	LvProgress::Finish(id);

	return true;
}

bool LvProjectGenerator::CleanBuild(const char* projectPath, const bool showProgress)
{
	if (!buildState.IsIdle())
	{
		// @donghun 이미 빌드 중
		LV_LOG(warning, "Already building");
		return false;
	}

	buildState.start();
	
	bool onCancel = false;
	bool onCancelResult = true;
	const int32 id = LvProgress::Start("User project", nullptr, ProgressOptions::INDEFINITE);

	static uint32 s_start = 0;
	s_start = lv_time_milli();
	static LvString s_title = "Build";
	static float s_progress = 0.0f;
	static const char* s_message = "";

	if (showProgress)
	{
		LvDispatchQueue("Refresh title").Async([]
			{
			lv_editor_get_window()->DisplayProgressBar(0.f, "", "", "CleanBuild");
				while (!buildState.IsIdle())
				{
					// @donghun 60fps 기준으로 매 프레임 갱신
					lv_thread_sleep(16);

					const uint32 current = lv_time_milli();
					const float second = static_cast<float>(current - s_start) / 1000.f;

					s_title.FormatSelf("Build (%.3f sec)", second);

					if (!buildState.IsIdle())
					{
						lv_editor_get_window()->DisplayProgressBar(s_progress, s_title.c_str(), s_message);
					}
				}
			});
	}

	LvProgress::RegisterCancelCallback(id, [&]
		{
			onCancel = true;
			return onCancelResult;
		});

	auto displayProgressBar = [](const float progress, const char* message)
	{
		s_progress = progress;
		s_message = message;
	};

	auto isCancel = [&]()
	{
		return onCancel || buildState.IsCancel();
	};

	do
	{
		float i = 0;
		constexpr float task = 5;

		displayProgressBar(i++ / task, "CMake Clean...");
		LvProgress::Report(id, "CMake Clean...");
		cmakeClean(projectPath);

		if (!cmakeListsUpdate(projectPath))	onCancel = true;
		if (isCancel()) break;

		displayProgressBar(i++ / task, "CMake Generate...");
		LvProgress::Report(id, "CMake Generate...");
		cmakeGenerate(projectPath);
		if (isCancel()) break;

		displayProgressBar(i++ / task, "Reflection Generate...");
		LvProgress::Report(id, "Reflection Generate...");
		if (!buildReflection()) break;

		if (isCancel()) break;

		displayProgressBar(i++ / task, "CMake Generate...");
		LvProgress::Report(id, "CMake Generate...");
		cmakeGenerate(projectPath);
		if (isCancel()) break;

		displayProgressBar(i++ / task, "Project Build...");
		LvProgress::Report(id, "Project Build...");
		if (0 != CoreCLR::ProjectManager::BuildProject(projectPath))
		{
			LV_LOG(error, "Build failed");
		}
	} while (false);

	bool result = true;
	onCancelResult = false;
	if (isCancel())
	{
		LvProgress::Finish(id, Status::CANCELED);
		result = false;
	}
	LvProgress::Finish(id);	

	lv_editor_get_window()->ClearProgressBar("CleanBuild");

	buildState.finish();
	LV_LOG(debug, "Build Complete");
	return result;
}

typedef LvApplicable* (LV_CALL* lv_create_app_entry)(Render::LvRenderContext* renderContext);

bool LvProjectGenerator::buildReflection()
{
	LvString args = "--build";
	const auto projectContext = lv_project_get_context();
	args = args.AppendFormat(" \"%s\" --config Debug --target", projectContext->solutionDirectoryPath.c_str());

	const LvString reflectionApplication = args.AppendFormat(" ReflectionApplication");
	if (0 != CoreCLR::ProjectManager::ExecuteCmake(reflectionApplication.c_str()))
	{
		LV_LOG(error, "Reflection Application build failed (HOW TO FIX : Remove the 'Program/native/src/app/CMakeLists.txt' and try again)");
		return false;
	}

	const LvString reflectionEditor = args.AppendFormat(" ReflectionEditor");
	if (0 != CoreCLR::ProjectManager::ExecuteCmake(reflectionEditor.c_str()))
	{
		LV_LOG(error, "Reflection Editor build failed (HOW TO FIX : Remove the 'Program/native/src/editor/CMakeLists.txt' and try again)");
		return false;
	}
	return true;
}

LV_NS_EDITOR_END
