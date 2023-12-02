#pragma once

#include "Renderer/FrameBuffer.h"

namespace Hazel
{
	class OpenGLFrameBuffer : public FrameBuffer 
	{
	public :
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer() override;

		virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; };
		virtual const FrameBufferSpecification& GetSpecification() override {return m_Specification;}
		void Invalidate();
		void Bind();
		void UnBind();
		virtual void Resize(uint32_t width, uint32_t height) override;
	private:
		uint32_t m_RendererID = 0;
		// Customizing  가능.  FrameBuffer 에 Color 를 붙이고 싶지 않을 수 있다.
		// 그저 Depth Buffer 용도로만 사용하고 싶을 수 있다.
		uint32_t m_ColorAttachment = 0;
		uint32_t m_DepthAttachment = 0;
		FrameBufferSpecification m_Specification;
	};
}


