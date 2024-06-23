#include "Hazel/Core/EngineContext.h"
#include <string>
#include "Hazel/Core/Application/Application.h"
#include "Hazel/Core/Core.h"

namespace Hazel
{
const char* EngineContext::Directories::defaultAssets = "DefaultAssets";

const char *EngineContext::ResourceDirectories::fonts = "fonts";
const char *EngineContext::ResourceDirectories::textures = "textures";
const char *EngineContext::ResourceDirectories::shaders = "shaders";

std::string EngineContext::m_DefaultAssetsPath;
// std::string EngineContext::m_DefaultResourcePath;
std::string EngineContext::m_ResourceRootPath;

void EngineContext::Initialize()
{
	// Application 의 Context 를 가져온다.
    const ApplicationContext &applicationContext = Application::Get().GetSpecification();

	const ApplicationCommandLineArgs &applicationCommandLineArgs =
        applicationContext.GetCommandLineArgs();

	const std::string &execPath = applicationCommandLineArgs[0];

    size_t buildPos = execPath.find("build");

    HZ_CORE_ASSERT(buildPos != std::string::npos, "Invalid exec path");

    // Find the last directory separator before "build"
    // ex) D:\DirectXPersonalProjectFolder\EngineSeriesTutorials\HazelGameEngine\build\Debug\Editor.exe
    // ex) -> D:\DirectXPersonalProjectFolder\EngineSeriesTutorials\HazelGameEngine\

    size_t lastDirSeparator = execPath.rfind('\\', buildPos);

    // Extract the substring before "build" including the directory separator
    std::string resourcePath = execPath.substr(0, lastDirSeparator + 1);

    resourcePath += "BJResource\\";

    m_ResourceRootPath = resourcePath;

    // m_DefaultResourcePath = resourcePath + "";

    m_DefaultAssetsPath = resourcePath + "DefaultAssets\\";

}
} // namespace Hazel
