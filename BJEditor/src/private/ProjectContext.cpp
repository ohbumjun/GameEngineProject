#include "ProjectContext.h"
#include "EditorContext.h"
#include "Hazel/FileSystem/DirectorySystem.h"
#include "File/FileManager.h"
#include "Project.h"
#include "EditorAsset/EditorAssetManager.h"

namespace HazelEditor
{
ProjectContext *s_projectContext = nullptr;

ProjectContext::ProjectContext(const char *absolutePath) : project(nullptr)
{
    builtinDirectoryPath =
        Hazel::DirectorySystem::CombinePath(absolutePath,
        EditorContext::Directories::builtin);

    programDirectoryPath =
        Hazel::DirectorySystem::CombinePath(absolutePath, EditorContext::Directories::program);
    
    settingDirectoryPath =
        Hazel::DirectorySystem::CombinePath(absolutePath, EditorContext::Directories::settings);
    
    assetCacheDirectoryPath = Hazel::DirectorySystem::CombinePath(
        absolutePath,
        EditorContext::Directories::library);

    assetCacheDirectoryPath = Hazel::DirectorySystem::CombinePath(
        assetCacheDirectoryPath.c_str(),
        EditorContext::Directories::Cache::asset);
    
    reflectCacheDirectoryPath = Hazel::DirectorySystem::CombinePath(
        absolutePath,
        EditorContext::Directories::library);

    reflectCacheDirectoryPath = Hazel::DirectorySystem::CombinePath(
        reflectCacheDirectoryPath.c_str(),
        EditorContext::Directories::Cache::reflect);

    solutionDirectoryPath =
        Hazel::DirectorySystem::CombinePath(absolutePath, EditorContext::Directories::solution);
    
    resourcesDirectoryPath =
        Hazel::DirectorySystem::CombinePath(absolutePath, EditorContext::Directories::resources);
    
    bundleDirectoryPath =
        Hazel::DirectorySystem::CombinePath(absolutePath, EditorContext::Directories::bundle);
}

Project *ProjectContext::Initialize(const std::string& absolutePath)
{
    HZ_CORE_ASSERT(nullptr == s_projectContext, "Duplicate calls");

    // FileManager::InitializeProjectPath(absolutePath);
    FileManager::Initialize(absolutePath.c_str());

    // EditorAssetManager::Initialize(&LvAssetProcessor::GetPolicy());

    // LvObjectAddress::SetFinder(&LvEditorScene::GetFinder());
    // LvObjectLabelTable::SetFinder(&LvBundleManifestManager::GetFinder());

    s_projectContext = new ProjectContext(absolutePath.c_str());
    s_projectContext->project =  new Project(absolutePath.c_str());

    Project *project = s_projectContext->project;

    EditorContext *editorContext = BJ_GetEditorContext();
    editorContext->m_Project = project;

    auto iterator = std::find(editorContext->m_Settings.m_LastOpenProjects.begin(),
                        editorContext->m_Settings.m_LastOpenProjects.end(),
                        absolutePath);
    const int index =
        iterator == editorContext->m_Settings.m_LastOpenProjects.end() ?
        -1 :
std::distance(editorContext->m_Settings.m_LastOpenProjects.begin(), iterator);

    if (-1 != index)
    {
        editorContext->m_Settings.m_LastOpenProjects.erase(
            editorContext->m_Settings.m_LastOpenProjects.begin() + index);
    }

    {
        auto it = editorContext->m_Settings.m_LastOpenProjects.begin() + 0;
        editorContext->m_Settings.m_LastOpenProjects.insert(it, absolutePath);
    }
    
    if (5 < editorContext->m_Settings.m_LastOpenProjects.size())
    {
        editorContext->m_Settings.m_LastOpenProjects.resize(5);
    }
    editorContext->SaveSettings();

    // Engine Context Setting
    {
        // LvEngineContext *engineContext = lv_engine_get_context();
        // engineContext->resourcesDirectoryPath =
        //     Hazel::DirectorySystem::CombinePath(absolutePath.c_str(),
        //                     LvEngineContext::Directories::resources);
    }

    const std::string libraryPath = Hazel::DirectorySystem::CombinePath(
        absolutePath.c_str(),
                        EditorContext::Directories::library);

    if (!Hazel::DirectorySystem::ExistDirectoryPath(s_projectContext->bundleDirectoryPath.c_str()))
    {
        Hazel::DirectorySystem::CreateDirectoryPath(
            s_projectContext->bundleDirectoryPath.c_str());
    }

    if (!Hazel::DirectorySystem::ExistDirectoryPath(
            s_projectContext->builtinDirectoryPath.c_str()))
    {
        Hazel::DirectorySystem::CreateDirectoryPath(
            s_projectContext->builtinDirectoryPath.c_str());
    }

    if (!Hazel::DirectorySystem::ExistDirectoryPath(libraryPath.c_str()))
    {
        Hazel::DirectorySystem::CreateDirectoryPath(libraryPath.c_str());
    }

    if (!Hazel::DirectorySystem::ExistDirectoryPath(
            s_projectContext->assetCacheDirectoryPath.c_str()))
    {
        Hazel::DirectorySystem::CreateDirectoryPath(
            s_projectContext->assetCacheDirectoryPath.c_str());
    }


    if (!Hazel::DirectorySystem::ExistDirectoryPath(
            s_projectContext->reflectCacheDirectoryPath.c_str()))
    {
        Hazel::DirectorySystem::CreateDirectoryPath(
            s_projectContext->reflectCacheDirectoryPath.c_str());
    }

    if (!Hazel::DirectorySystem::ExistDirectoryPath(
            s_projectContext->solutionDirectoryPath.c_str()))
    {
        Hazel::DirectorySystem::CreateDirectoryPath(
            s_projectContext->solutionDirectoryPath.c_str());
    }

    return s_projectContext->project;
}

void ProjectContext::Finalize(Project *project)
{
    // 메인 쓰레드에서만 !
    // LV_CHECK_MAIN_THREAD();

    // 이전에 열려있는 Project가 있다면, 해당 Project에서 작업하고 있는 Setting Data는
    // 해당 Project Setting 파일에 저장시켜야 한다.
    // 에디터가 종료되기 전에 현재 Project Setting Data를 파일로 저장시킨다.
    project->SaveSettings();

    // 현재 프로젝트를 닫기 전에 현재 작업중인 Scene이 있으면 Serialize하고 마무리한다.
    // if (LvEditorSceneManager::GetActive() != nullptr)
    // {
    //     LvAsset *sceneAsset = LvAssetDatabase::GetAssetByInstanceId(
    //         LvEditorSceneManager::GetActive()
    //             ->GetContainer()
    //             ->host->GetInstanceId());
    //     LV_CHECK(sceneAsset, "LvScene Asset is not created. Wrong Logic!");
    // 
    //     // 이미 만들어졌던 Scene 저장되게하기
    //     LvAssetDatabase::SaveAsset(sceneAsset);
    //     //현재 Scene Clear
    //     // - RenderDatabase에 data refCount -1
    //     LvEditorSceneManager::GetActive()->Finalize();
    //     // 위에 저장된 Scene은 LvAssetdatabase에서 관리되어서 해제되므로, 이 context에서는 nullptr 처리함
    //     LvEditorSceneManager::SetActive(nullptr);
    // }

    EditorAssetManager::Finalize();

    delete s_projectContext;
    delete project;
    s_projectContext = nullptr;

    FileManager::Finalize();

    // LvObjectAddress::SetFinder(nullptr);
}

ProjectContext *BJ_GetProjectContext()
{
    return s_projectContext;
}

} // namespace HazelEditor
