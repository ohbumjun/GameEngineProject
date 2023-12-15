#pragma once

#include "Hazel/Core/Core.h"

namespace Hazel
{
	// FramebufferFormat : ex) Color Buffer, Depth Buffer, Shadow Map...
	enum class FrameBufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER, // int 

		// Depth, stencil
		DEPTH24STENCIL8,

		// Default
		DEPTH = DEPTH24STENCIL8
	};

	// 실제 Frame buffer 로 사용할 Texture 의 format 을 결정하는 구조체
	struct FrameBufferTextureSpecification
	{
		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification(FrameBufferTextureFormat format) :
			m_TextureFormat(format) {}

		// 해당 Frame Buffer 가 어떤 format 인가
		FrameBufferTextureFormat m_TextureFormat = FrameBufferTextureFormat::None;
	};

	// 모든 종류의 FrameBufferTextureSpecification 를 모아둔 것
	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachment) :
			m_Attachment(attachment){}

		// 모든 종류의 FrameBufferTextureSpecification 가 될 것이다.
		// ex) 어떤 Frame Buffer 의 설정값에는 Color Buffer 관련 2개, Depth Buffer 1개와 같이
		//      여러 설정값들이 들어있을 수 있다.
		std::vector<FrameBufferTextureSpecification> m_Attachment;
	};

	// 각 Frame Buffer 마다 하나의 FrameBufferSpecification 를 가지고 있을 것이다.
	struct FrameBufferSpecification
	{
		uint32_t Width, Height;
		
		// > 1 ? 멀티 샘플링이라는 의미가 된다.
		uint32_t Samples = 1;

		// false : glBindFrameBuffer(0) 와 같은 의미이다
		// 즉, 모든 FrameBuffer  를 Unblnd 시키고 Screen 에 직접 그리겠다
		//      다른 말로 하면, 실제 존재하지 않는 FrameBuffer 를 알아서 만들어주는 것이라고도 할 수 있다.
		bool SwapChainTarget = false;

		FrameBufferAttachmentSpecification Attachments;
	};

	class FrameBuffer
	{
	public :
		virtual ~FrameBuffer() {};
		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int xCoord, int yCoord) = 0;
		virtual uint32_t GetColorAttachmentRendererID(uint32 index = 0) const = 0;
		virtual const FrameBufferSpecification& GetSpecification() = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};
}



