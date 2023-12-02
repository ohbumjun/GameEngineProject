#pragma once

#include "Hazel/Core/Core.h"

namespace Hazel
{
	struct FrameBufferSpecification
	{
		uint32_t Width, Height;
		// FramebufferFormat : ex) Color Buffer, Depth Buffer, Shadow Map...
		
		uint32_t Samples = 1;

		// false : glBindFrameBuffer(0) 와 같은 의미이다
		// 즉, 모든 FrameBuffer  를 Unblnd 시키고 Screen 에 직접 그리겠다
		//      다른 말로 하면, 실제 존재하지 않는 FrameBuffer 를 알아서 만들어주는 것이라고도 할 수 있다.
		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public :
		virtual ~FrameBuffer() {};
		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		virtual const FrameBufferSpecification& GetSpecification() = 0;

		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};
}



