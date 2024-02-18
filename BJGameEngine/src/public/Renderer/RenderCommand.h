#pragma once

#include "RendererAPI.h"

namespace Hazel
{
	// 실제 raw render related command 를 처리하는 class
class HAZEL_API RenderCommand
	{
	public :
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}
		inline static void Init()
		{
			s_RendererAPI->Init();
		}
		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, 
			uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}
		inline static void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewPort(x, y, width, height);
		}

		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_RendererAPI->DrawLines(vertexArray, vertexCount);
		}

		static void SetLineWidth(float width)
		{
			s_RendererAPI->SetLineWidth(width);
		}
	private :
		static RendererAPI* s_RendererAPI;
	};

}


