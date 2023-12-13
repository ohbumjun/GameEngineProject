#pragma once

#include "Renderer/Buffer/Buffer.h"

namespace Hazel
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public :
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		OpenGLVertexBuffer(uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetData(const void* data, uint32_t size);
		virtual void Bind() const override;
		virtual void Unbind() const override;

	private :
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual uint32_t GetCount() { return m_Count; }
		virtual void Bind() const;
		virtual void Unbind() const;

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}


