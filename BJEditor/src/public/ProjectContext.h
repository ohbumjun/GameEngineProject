#pragma once

namespace HazelEditor
{
class Project;

class ProjectContext
{
public:
    static Project *Initialize(const std::string& absolutePath);
    static void Finalize(Project *project);

    Project *project;

    std::string resourcesDirectoryPath;

    std::string bundleDirectoryPath;

    std::string programDirectoryPath;

    std::string settingDirectoryPath;

    std::string assetCacheDirectoryPath;

    std::string builtinDirectoryPath;

    std::string reflectCacheDirectoryPath;

    std::string solutionDirectoryPath;

private:
    ProjectContext(const char *absolutePath);
};

/**
 * @brief 현재 열려있는 프로젝트의 컨텍스트 반환, 열려있지 않은 경우 nullptr
 * @file #include "editor/project/LvProject.h"
 */
ProjectContext * BJ_GetProjectContext();

} // namespace HazelEditor
