#pragma once

#include "Renderer/FrameBuffer.h"

namespace Hazel
{
	class OpenGLFrameBuffer : public FrameBuffer 
	{
	public :
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();
		virtual const FrameBufferSpecification& GetSpecification() override {}	
		void Invalidate();
	private:
		uint32_t m_RendererID;
		// Customizing  가능.  FrameBuffer 에 Color 를 붙이고 싶지 않을 수 있다.
		// 그저 Depth Buffer 용도로만 사용하고 싶을 수 있다.
		uint32_t m_ColorAttachment;
		FrameBufferSpecification m_Specification;
	};
}


