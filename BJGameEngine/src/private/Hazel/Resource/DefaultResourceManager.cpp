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

// specificAssetFolderPath : Ư�� type �� asset ��� �Ʒ� �κ�
// ex) DefaultAssets/shaders/ ~~ 
BaseObject* loadDefaultAsset(const std::string& specificAssetFolderPath)
{
    AssetManagerController *assetManagerCtrl =
        AssetManagerController::GetInst();

    // for �� ���鼭 ������ �ִ� ��� ���ϵ��� ��Ƽ�
    // import ���ش�. (���� : 1 depth �θ� �����Ѵ�)
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
        // �ش� ��ο� �ִ� ������ Load �Ѵ�.
        // AssetManager �� ������ش�.
        assetManagerCtrl->LoadAsset(specificAssetFolderPath);

        // cache �� ����. -> �̰Ŵ� Editor ��ɿ��� overrride �Ͽ� �����ϱ�

	}

   

    return nullptr;
}

void DefaultResourceManager::LoadDefaultResources()
{
	// engine context �� ���õ� path �� ���� ������ Ȱ���Ͽ�
	// ��� asset ���� �̸� Load �Ѵ�.
	// �̶� ����ϴ� ���� AssetManagerBase �� �Լ����� Ȱ���Ѵ�.

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

    // �� ������ �̸� ? �κи��� �����Ѵ�.
    // Engine Context �� ���ǵ� asset type ���� import �� �Ѵ�.
    for (const auto &subfolder : subfolders)
	{
		const std::string& subfolderName = subfolder.string();

        // �ϴ� ����� Ư�� type �� default asset �鸸 load �ϰ� �ϱ� ���ؼ�
        // folder file name �� �˻��Ͽ� load ���� ������ ������ ���̴�.
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
            // texture ���·� ��� ?
            loadDefaultAsset(subfolderName);
        }
        else if (lastPartStr == EngineContext::ResourceDirectories::shaders)
        {
            // ���������� texture ���·� ��� ?
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
