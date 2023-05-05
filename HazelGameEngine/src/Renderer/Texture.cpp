#include "hzpch.h"
#include "Texture.h"
#include "Hazel/Core/Core.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"

namespace Hazel
{
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "no api set");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(path);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}