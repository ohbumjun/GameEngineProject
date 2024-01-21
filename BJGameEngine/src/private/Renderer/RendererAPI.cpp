#include "hzpch.h"
#include "Renderer/RendererAPI.h"

namespace Hazel
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
	inline void RendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
	}
}