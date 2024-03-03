#pragma once

#include "VertexArray.h"
#include <glm/glm.hpp>

namespace Hazel
{
class HAZEL_API RendererAPI
	{
	public :
        enum class HAZEL_API API
		{
			None = 0, OpenGL = 1
		};

	public :
		virtual ~RendererAPI() {};
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void Init() = 0;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray,
			uint32_t indexCount = 0) = 0;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
		virtual void SetLineWidth(float width) = 0;
		static API GetAPI() { return s_API; }
	private :
        static API s_API;
	};
}


