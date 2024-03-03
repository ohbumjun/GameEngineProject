#pragma once

#include "Renderer/Buffer/VertexArray.h"

namespace Hazel
{
	class OpenGLVertexArray : public VertexArray
	{
		friend class VertexArray;

	public :
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual void AddVertexBuffer(std::shared_ptr<VertexBuffer>& buffer) override;
		virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer>& buffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer() const override { return m_VertexBuffers; }
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexedBuffer; };

	private :
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexedBuffer;
		uint32_t m_VertexBufferIndex = 0;
	};
}



