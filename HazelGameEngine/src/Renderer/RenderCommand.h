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
		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private :
		static RendererAPI* s_RendererAPI;
	};

}


