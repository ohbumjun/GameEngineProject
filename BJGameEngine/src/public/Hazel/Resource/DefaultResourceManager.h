#pragma once

#include <string>
#include "Hazel/Resource/ResourceContainer.h"

namespace Hazel
{
	// 차후 Engine Pack 용
	class DefaultResourceLoader
	{

	};

	class DefaultResourceManager
	{
    public:
        static void Initialize(const char *defaultResourcePath);
        static void LoadDefaultResources();
		// deafult asset 경로 세팅되어야 한다.
    private:
        static std::string m_DefaultAssetPath;
        DefaultResourceLoader* m_DefaultResourceLoader;
	};
} // namespace Hazel
