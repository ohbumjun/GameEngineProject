#pragma once

#include "Hazel/Resource/ResourceContainer.h"
#include <string>

namespace Hazel
{
	class BaseObject;

	// ���� Engine Pack ��
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
		// deafult asset ��� ���õǾ�� �Ѵ�.
    private:
        static std::string m_DefaultAssetPath;

	};
} // namespace Hazel
