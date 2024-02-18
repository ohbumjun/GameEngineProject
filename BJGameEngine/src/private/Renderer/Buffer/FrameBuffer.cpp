#include "hzpch.h"
#include "Renderer/Buffer/FrameBuffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Hazel
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
		{
			HZ_CORE_ASSERT(false, "no api set");
			return nullptr;
		}
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<OpenGLFrameBuffer>(spec);
		}
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}

