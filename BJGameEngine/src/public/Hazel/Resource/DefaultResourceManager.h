#pragma once

#include <string>
#include "Hazel/Resource/ResourceContainer.h"

namespace Hazel
{
	// ���� Engine Pack ��
	class DefaultResourceLoader
	{

	};

	class DefaultResourceManager
	{
    public:
        static void Initialize(const char *defaultResourcePath);
        static void LoadDefaultResources();
		// deafult asset ��� ���õǾ�� �Ѵ�.
    private:
        static std::string m_DefaultAssetPath;
        DefaultResourceLoader* m_DefaultResourceLoader;
	};
} // namespace Hazel
