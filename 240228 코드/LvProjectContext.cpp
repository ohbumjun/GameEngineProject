#include "editor/project/LvProjectContext.h"
#include "LvPrecompiled.h"

#include "system/LvLog.h"

#include "engine/LvEngine.h"
#include "engine/LvObjectLabelTable.h"
#include "engine/thread/LvDispatchQueue.h"

#include "editor/LvEditorContext.h"
#include "editor/LvEditorScene.h"
#include "editor/LvEditorSceneGraph.h"
#include "editor/LvProjectPreferences.h"
#include "editor/LvSelection.h"
#include "editor/asset/LvAssetDatabase.h"
#include "editor/asset/LvAssetProcessor.h"
#include "editor/asset/LvPropertyReference.h"
#include "editor/asset/LvShaderAsset.h"
#include "editor/bundle/LvBundleManifestManager.h"
#include "editor/private/project/LvFileManager.h"

#include "editor/project/LvProject.h"
#include "editor/renderer/LvRenderDatabase.h"
#include "editor/scene/LvEditorSceneManager.h"

#include "editor/private/LvBuiltInDistribute.h"
#include "engine/asset/LvBundleRepository.h"


using namespace Lv;
using namespace Lv::Engine;
using namespace Lv::Editor::Project;
using EngineRDB = Engine::Renderer::LvRenderDatabase;
using EditorRDB = Editor::Renderer::LvRenderDatabase;

LV_NS_EDITOR_BEGIN

bool LvProjectContext::s_isTest = false;
bool LvProjectContext::s_isTestWithoutRender = false;

LvProjectContext *s_projectContext = nullptr;

LvProjectContext::LvProjectContext(const char *absolutePath) : project(nullptr)
{
    builtinDirectoryPath =
        lv_path_combine(absolutePath,
                        LvEditorContext::Directories::builtinresource);
    programDirectoryPath =
        lv_path_combine(absolutePath, LvEditorContext::Directories::program);
    settingDirectoryPath =
        lv_path_combine(absolutePath, LvEditorContext::Directories::settings);
    assetCacheDirectoryPath =
        lv_path_combine(absolutePath,
                        {LvEditorContext::Directories::library,
                         LvEditorContext::Directories::Cache::asset});
    assetPreviewCacheDirectoryPath =
        lv_path_combine(absolutePath,
                        {LvEditorContext::Directories::library,
                         LvEditorContext::Directories::Cache::assetPreview});
    nodeGraphLayoutCacheDirectoryPath =
        lv_path_combine(absolutePath,
                        {LvEditorContext::Directories::library,
                         LvEditorContext::Directories::Cache::nodeGraphLayout});
    reflectCacheDirectoryPath =
        lv_path_combine(absolutePath,
                        {LvEditorContext::Directories::library,
                         LvEditorContext::Directories::Cache::reflect});
    shaderCacheDirectoryPath =
        lv_path_combine(absolutePath,
                        {LvEditorContext::Directories::library,
                         LvEditorContext::Directories::Cache::shader});
    builtInShaderCacheDirectoryPath =
        lv_path_combine(absolutePath,
                        {LvEditorContext::Directories::library,
                         LvEditorContext::Directories::Cache::builtInShader});
    infCacheDirectoryPath =
        lv_path_combine(absolutePath,
                        {LvEditorContext::Directories::library,
                         LvEditorContext::Directories::Cache::inf});
    platformAssetCacheDirectoryPath =
        lv_path_combine(absolutePath,
                        {LvEditorContext::Directories::library,
                         LvEditorContext::Directories::Cache::platformAsset});
    solutionDirectoryPath =
        lv_path_combine(absolutePath, LvEditorContext::Directories::solution);
    resourcesDirectoryPath =
        lv_path_combine(absolutePath, LvEditorContext::Directories::resources);
    bundleDirectoryPath =
        lv_path_combine(absolutePath, LvEditorContext::Directories::bundle);
}

LvProject *LvProjectContext::Initialize(const char *absolutePath,
                                        std::function<void()> &&onDestoryed)
{
    LV_CHECK(nullptr == s_projectContext, "Duplicate calls");
    LvShaderNameMap::Clear();

    // @author bumjunoh
    // Project Migration 이 진행되고 나서 FileWatcher 를 실행하게 해야 한다.
    // 그렇지 않으면 Migration 과정에서 바뀐 모든 파일들이 FileWatcher 에 의해 처리된다.
    // 따라서 Migration 이 모두 끝나고 나서 FileManager::Initialize 를 호출하도록 수정한다.
    // 단, project->MigrateVersion 함수 내에서 FileManager 의 _projectPath 변수를 사용하기 때문에, projectPath 변수만 초기화 먼저 한다.
    LvFileManager::InitializeProjectPath(absolutePath);

    LvBundleRepository::Initialize();
    LvAssetDatabase::Initialize(&LvAssetProcessor::GetPolicy());

    LvAssetDatabase::IsUsable = []() -> bool {
        return lv_project_get_context()->project->GetPlayState() ==
               LvProject::PlayState::PLAY;
    };

    // LvObjectAddress::SetFinder(&LvAssetDatabase::finder);
    LvObjectAddress::SetFinder(&LvEditorScene::GetFinder());
    LvObjectLabelTable::SetFinder(&LvBundleManifestManager::GetFinder());

    s_projectContext = new LvProjectContext(absolutePath);
    s_projectContext->project =
        new LvProject(absolutePath, std::move(onDestoryed));

    LvProjectPreferences::Initialize();

    LvProject *project = s_projectContext->project;

    if (false == LvProjectContext::s_isTest)
    {

        LvEditorContext *editorContext = lv_editor_get_context();
        editorContext->settings.autoCreateProject =
            lv_path_parent(absolutePath);
        editorContext->project = project;

        const int index =
            editorContext->settings.lastOpenProjects.IndexOf(absolutePath);
        if (-1 != index)
        {
            editorContext->settings.lastOpenProjects.RemoveAt(index);
        }
        editorContext->settings.lastOpenProjects.Insert(0, absolutePath);
        if (5 < editorContext->settings.lastOpenProjects.Count())
        {
            editorContext->settings.lastOpenProjects.Resize(5);
        }
        editorContext->SaveSettings();

        LvEngineContext *engineContext = lv_engine_get_context();
        // Engine Context Setting
        {
            engineContext->resourcesDirectoryPath =
                lv_path_combine(absolutePath,
                                LvEngineContext::Directories::resources);
            engineContext->sceneGraphPath =
                s_projectContext->settings.defaultSceneGraphFileName;
            engineContext->bundleDirectoryPath =
                lv_path_combine(absolutePath,
                                LvEngineContext::Directories::bundle);
        }

        const LvString libraryPath =
            lv_path_combine(absolutePath,
                            LvEditorContext::Directories::library);

        if (!lv_directory_exist(s_projectContext->bundleDirectoryPath.c_str()))
        {
            lv_directory_create(s_projectContext->bundleDirectoryPath.c_str());
        }

        if (!lv_directory_exist(s_projectContext->builtinDirectoryPath.c_str()))
        {
            lv_directory_create(s_projectContext->builtinDirectoryPath.c_str());
        }

        if (!lv_directory_exist(libraryPath.c_str()))
        {
            lv_directory_create(libraryPath.c_str());
        }

        if (!lv_directory_exist(
                s_projectContext->assetCacheDirectoryPath.c_str()))
        {
            lv_directory_create(
                s_projectContext->assetCacheDirectoryPath.c_str());
        }

        if (!lv_directory_exist(
                s_projectContext->assetPreviewCacheDirectoryPath.c_str()))
        {
            lv_directory_create(
                s_projectContext->assetPreviewCacheDirectoryPath.c_str());
        }

        if (!lv_directory_exist(
                s_projectContext->nodeGraphLayoutCacheDirectoryPath.c_str()))
        {
            lv_directory_create(
                s_projectContext->nodeGraphLayoutCacheDirectoryPath.c_str());
        }

        if (!lv_directory_exist(
                s_projectContext->reflectCacheDirectoryPath.c_str()))
        {
            lv_directory_create(
                s_projectContext->reflectCacheDirectoryPath.c_str());
        }

        //if (Engine::LvBuiltInObject::needShaderCacheClear && lv_directory_exist(s_projectContext->shaderCacheDirectoryPath.c_str()))
        //{
        //	lv_directory_remove(s_projectContext->shaderCacheDirectoryPath.c_str());
        //}

        //if (Engine::LvBuiltInObject::needShaderCacheClear && lv_directory_exist(s_projectContext->builtInShaderCacheDirectoryPath.c_str()))
        //{
        //	lv_directory_remove(s_projectContext->builtInShaderCacheDirectoryPath.c_str());
        //}

        if (!lv_directory_exist(
                s_projectContext->shaderCacheDirectoryPath.c_str()))
        {
            lv_directory_create(
                s_projectContext->shaderCacheDirectoryPath.c_str());
        }

        if (!lv_directory_exist(
                s_projectContext->builtInShaderCacheDirectoryPath.c_str()))
        {
            lv_directory_create(
                s_projectContext->builtInShaderCacheDirectoryPath.c_str());
        }

        if (!lv_directory_exist(
                s_projectContext->infCacheDirectoryPath.c_str()))
        {
            lv_directory_create(
                s_projectContext->infCacheDirectoryPath.c_str());
        }

        if (!lv_directory_exist(
                s_projectContext->platformAssetCacheDirectoryPath.c_str()))
        {
            lv_directory_create(
                s_projectContext->platformAssetCacheDirectoryPath.c_str());
        }

        if (!lv_directory_exist(
                s_projectContext->solutionDirectoryPath.c_str()))
        {
            lv_directory_create(
                s_projectContext->solutionDirectoryPath.c_str());
        }

        LvBundleManifestManager::Initialize();
    }

    return s_projectContext->project;
}

void LvProjectContext::Finalize(LvProject *project)
{
    LV_CHECK_MAIN_THREAD();

    // 이전에 열려있는 Project가 있다면, 해당 Project에서 작업하고 있는 Setting Data는
    // 해당 Project Setting 파일에 저장시켜야 한다.
    // 에디터가 종료되기 전에 현재 Project Setting Data를 파일로 저장시킨다.
    if (false == LvProjectContext::s_isTest)
    {
        project->SaveSettings();
    }

    LvSelection::Clear(false);
    LvSelection::Finalize();

    // @yuiena 03/25
    // scene은 열려있는거만 저장하는데 Manifest는 전부 저장하고 있어서 데이터가 안맞는 문제가 있다.
    // 열려있지 않는 scene이라도 변경점이 있다면 isChange, SetChangeState 함수등을 통해 바꾸고 체크해야한다.
    // 아래도 만약에 굳이 변경점이 없는 Scene이라면 저장하지 않아도 되지만 현재는 Scene이 변경점을 제대로 체크하고있지 않아 그대로 둔다.

    // 현재 프로젝트를 닫기 전에 현재 작업중인 Scene이 있으면 Serialize하고 마무리한다.
    if (LvEditorSceneManager::GetActive() != nullptr)
    {
        LvAsset *sceneAsset = LvAssetDatabase::GetAssetByInstanceId(
            LvEditorSceneManager::GetActive()
                ->GetContainer()
                ->host->GetInstanceId());
        LV_CHECK(sceneAsset, "LvScene Asset is not created. Wrong Logic!");

        // 이미 만들어졌던 Scene 저장되게하기
        LvAssetDatabase::SaveAsset(sceneAsset);
        //현재 Scene Clear
        // - RenderDatabase에 data refCount -1
        LvEditorSceneManager::GetActive()->Finalize();
        // 위에 저장된 Scene은 LvAssetdatabase에서 관리되어서 해제되므로, 이 context에서는 nullptr 처리함
        LvEditorSceneManager::SetActive(nullptr);
    }
    LvEditorSceneGraph::Finalize();

    // Scene이나 다른 Asset다 내려가기전에 manifest 내려가면 안된다. scene이 manifest 데이터를 참조를 하기 때문.
    // 현재 올라와 있는 BundleManifest를 모두 Serialize시키고, 비워놓고, ObjectTable를 모두 비워놓는다.
    LvBundleManifestManager::Finalize();

    if (LvAssetProcessor::IsInitialized())
        LvAssetProcessor::Finalize();

    if (false == LvProjectContext::s_isTest)
    {
        EditorRDB *editorRDB = lv_editor_get_context()->renderDatabase;
        if (editorRDB != nullptr)
        {
            editorRDB->Clear();
        }
    }


    if (s_projectContext != nullptr)
    {
        if (LvAssetDatabase::IsUsable)
            LvAssetDatabase::IsUsable = nullptr;
        LvAssetDatabase::Finalize();

        LvFallbackMaterial::Finalize();

        delete s_projectContext;
        delete project;
        s_projectContext = nullptr;

        LvFileManager::Finalize();
        LvShaderNameMap::Clear();
        LvPropertyReference::Container::Finalize();
    }
    else
    {
        LV_LOG(warning, "Context is nullptr");
    }
    LvBundleRepository::Finalize();

    LvProjectPreferences::Finalize();
    LvObjectAddress::SetFinder(nullptr);
    LvObjectLabelTable::SetFinder(nullptr);
}

LvProjectContext *lv_project_get_context()
{
    return s_projectContext;
}


LV_NS_EDITOR_END
