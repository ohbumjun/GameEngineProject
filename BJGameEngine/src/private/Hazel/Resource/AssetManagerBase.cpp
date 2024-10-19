#include "Hazel/Resource/AssetManagerBase.h"

namespace Hazel
{
AssetManagerController *AssetManagerController::s_Instance = nullptr;

void AssetManagerController::Initialize(AssetManagerController *assetManager)
{
    HZ_CORE_ASSERT(assetManager, "AssetManager is nullptr");
    HZ_CORE_ASSERT(s_Instance == nullptr, "AssetManager Already Initialized");

    s_Instance = assetManager;
}
void AssetManagerController::LoadAsset(const std::string &relativePath)
{
    s_Instance->loadAsset(relativePath);
};

} // namespace Hazel
