#pragma once

#include "RendererAPI.h"

namespace Hazel
{
	// 실제 raw render related command 를 처리하는 class
	class RenderCommand
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
		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
		inline static void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewPort(x, y, width, height);
		}
	private :
		static RendererAPI* s_RendererAPI;
	};

}


