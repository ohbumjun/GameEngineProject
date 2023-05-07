#pragma once

#include "Renderer/RendererAPI.h"

namespace Hazel
{
	class OpenGLRendererAPI : public RendererAPI
	{
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void Init() override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	};
}


