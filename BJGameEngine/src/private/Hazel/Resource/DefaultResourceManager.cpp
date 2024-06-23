#include "Hazel/Resource/DefaultResourceManager.h"

namespace Hazel
{
std::string DefaultResourceManager::m_DefaultAssetPath = "";

void DefaultResourceManager::Initialize(const char *defaultResourcePath)
{
	m_DefaultAssetPath = defaultResourcePath;
}

} // namespace Hazel
