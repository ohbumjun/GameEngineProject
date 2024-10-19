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
}

Project *ProjectContext::Initialize(const std::string& absolutePath)
{
    HZ_CORE_ASSERT(nullptr == s_projectContext, "Duplicate calls");

    FileManager::Initialize(absolutePath.c_str());

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

    const std::string libraryPath = Hazel::DirectorySystem::CombinePath(
        absolutePath.c_str(),
                        EditorContext::Directories::library);

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
    // 이전에 열려있는 Project가 있다면, 해당 Project에서 작업하고 있는 Setting Data는
    // 해당 Project Setting 파일에 저장시켜야 한다.
    // 에디터가 종료되기 전에 현재 Project Setting Data를 파일로 저장시킨다.
    project->SaveSettings();

    EditorAssetManager::Finalize();

    delete s_projectContext;
    delete project;
    s_projectContext = nullptr;

    FileManager::Finalize();
}

ProjectContext *BJ_GetProjectContext()
{
    return s_projectContext;
}

} 
