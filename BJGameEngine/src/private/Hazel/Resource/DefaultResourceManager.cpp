#include "Hazel/Resource/DefaultResourceManager.h"

namespace Hazel
{
void DefaultResourceManager::Initialize(const char *defaultResourcePath)
{
	m_DefaultAssetPath = defaultResourcePath;
}

} // namespace Hazel
