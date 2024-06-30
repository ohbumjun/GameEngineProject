#pragma once

namespace HazelEditor
{
class Project;

class ProjectContext
{
public:
    static Project *Initialize(const std::string& absolutePath);
    static void Finalize();

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
} // namespace HazelEditor
