#pragma once

#include "Hazel/Resource/ResourceContainer.h"
#include <string>

namespace Hazel
{
	class BaseObject;

	// 차후 Engine Pack 용
	class DefaultResourceLoader
	{
	};

	enum class DefaultShaderType
    {
        DEFAULT_SHADER
    };

	class DefaultResourceManager
	{
    public:
        static void LoadDefaultResources();
        static BaseObject *GetDefaultShader(ResourceType resourceType, DefaultShaderType shaderType);
		// deafult asset 경로 세팅되어야 한다.
    private:
        static std::string m_DefaultAssetPath;

	};
} // namespace Hazel
