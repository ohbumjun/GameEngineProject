#pragma once

#include "Renderer/VertexArray.h"

namespace Hazel
{
	class OpenGLVertexArray : public VertexArray
	{
		friend class VertexArray;

		OpenGLVertexArray();
		virtual ~OpenGLVertexArray(){}

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual void AddVertexBuffer(std::shared_ptr<VertexBuffer>& buffer) override;
		virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer>& buffer) override;

	private :
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexedBuffer;
	};
}



