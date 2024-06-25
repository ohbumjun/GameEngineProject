#include "Hazel/Resource/DefaultResourceManager.h"
#include "Hazel/Core/EngineContext.h"
#include "Hazel/Resource/AssetManagerBase.h"
#include <filesystem>

namespace Hazel
{
std::string DefaultResourceManager::m_DefaultAssetPath = "";

// Function to recursively iterate through directories and collect subfolders
void getSubfolders(const std::filesystem::path &path,
                   std::vector<std::filesystem::path> &subfolders)
{
    if (!std::filesystem::exists(path))
    {
        std::cerr << "Error: Path does not exist: " << path << std::endl;
        return;
    }

    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
        if (std::filesystem::is_directory(entry))
        {
            subfolders.push_back(entry);
            getSubfolders(entry,
                          subfolders); // Recursive call for subdirectories
        }
    }
}

// specificAssetFolderPath : 특정 type 의 asset 경로 아래 부분
// ex) DefaultAssets/shaders/ ~~ 
BaseObject* loadDefaultAsset(const std::string& specificAssetFolderPath)
{
    AssetManagerController *assetManagerCtrl =
        AssetManagerController::GetInst();

    // for 문 돌면서 하위에 있는 모든 파일들을 모아서
    // import 해준다. (주의 : 1 depth 로만 세팅한다)
    std::vector<std::filesystem::path> subFiles;

    for (const auto &entry :
         std::filesystem::directory_iterator(specificAssetFolderPath))
    {
        if (std::filesystem::is_regular_file(entry))
        {
            subFiles.push_back(entry);
        }
    }

    for (const auto& subfile : subFiles)
    { 
        // 해당 경로에 있는 파일을 Load 한다.
        // AssetManager 에 등록해준다.
        assetManagerCtrl->LoadAsset(specificAssetFolderPath);

        // cache 로 쓴다. -> 이거는 Editor 기능에서 overrride 하여 진행하기

	}

   

    return nullptr;
}

void DefaultResourceManager::LoadDefaultResources()
{
	// engine context 에 세팅된 path 와 폴더 정보를 활용하여
	// 모든 asset 들을 미리 Load 한다.
	// 이때 사용하는 것은 AssetManagerBase 의 함수들을 활용한다.

	const std::string &defaultAssetFolderPath = EngineContext::GetDefaultAssetPath();

    std::filesystem::path startingPath(defaultAssetFolderPath);

    std::vector<std::filesystem::path> subfolders;

    for (const auto &entry : std::filesystem::directory_iterator(startingPath))
    {
        if (std::filesystem::is_directory(entry))
        {
            subfolders.push_back(entry);
        }
    }

    // 각 폴더의 이름 ? 부분만을 추출한다.
    // Engine Context 에 정의된 asset type 별로 import 를 한다.
    for (const auto &subfolder : subfolders)
	{
		const std::string& subfolderName = subfolder.string();

        // 일단 현재는 특정 type 의 default asset 들만 load 하게 하기 위해서
        // folder file name 을 검사하여 load 할지 말지를 결정할 것이다.
        size_t pos = subfolderName.rfind(std::string{'\\', '/'});

        // If a separator is found, extract the last part name
        std::string lastPart;

        HZ_CORE_ASSERT(pos != std::string::npos,
                       "Invalid asset path: {0}",
                       subfolderName);

        lastPart = subfolderName.substr(pos + 1);

        const char *lastPartStr = lastPart.c_str();

        if (lastPartStr == EngineContext::ResourceDirectories::textures)
        {
            // texture 형태로 등록 ?
            loadDefaultAsset(subfolderName);
        }
        else if (lastPartStr == EngineContext::ResourceDirectories::shaders)
        {
            // 마찬가지로 texture 형태로 등록 ?
            loadDefaultAsset(subfolderName);
        }

	}
}

BaseObject *DefaultResourceManager::GetDefaultShader(
    ResourceType resourceType,
	DefaultShaderType shaderType)
{
    return nullptr;
}

} // namespace Hazel
