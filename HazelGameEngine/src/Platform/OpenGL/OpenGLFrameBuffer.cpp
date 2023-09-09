#include "hzpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Hazel
{
	static const uint32_t s_MaxFrameBufferSize = 8192;

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_DepthAttachment);
	}

	// State is not valid, so recreate it
	void OpenGLFrameBuffer::Invalidate()
	{
		// RenderID 가 이미 세팅된 상황이라면
		if (m_RendererID != 0)
		{
			// 기존에 세팅되었던 것을 모두 지운다.
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);

			// 새로운 FrameBuffer 를 아래 함수들을 통해 만든다.
		}

		// Used to create framebuffer objects
		// A framebuffer is a memory buffer that stores pixel data for display
		// - allows you to perform various rendering operations and manipulations before displaying the final result on the screen
		glCreateFramebuffers(1, &m_RendererID);

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// 아래 과정은, FrameBuffer 를 위한 텍스처를 생성하는 과정이다.
		// 일반적인 텍스쳐 생성과정과 거의 동일하다. 다만, 텍스처의 크기를 스크린 크기로 설정한다는 것 + 마지막 nullptr 인자를 넣는다는 것
		// 해당 텍스쳐에 대해 우리는 오직 메모리만 할당하고, 실제로 채워넣지는 않는다. 왜냐하면, 텍스처를 채우는 행위는 우리가 frameBuffer 에
		// 렌더링 하면, 그때 가서 텍스쳐가 채워지는 것이기 때문이다.
		// 그리고 일반적인 Texture 생성때와 달리, wrapping method 나 mipmapping 을 신경쓰지 않아도 된다는 장점이 있다.

		// texture object 를 만들어주는 함수
		// + 만들어낸 Texture Object 를 가리키는 ID 리턴
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D,  m_ColorAttachment);

		// Texturing 을 통해 image array 가, shader 에 의해 읽게 될 수 있다.
		// glTexImage2D : Texture image 를 define 하기 위해 호출하는 함수. 해당 texture image 의 각종 속성을 parameter 를 통해 정한다.
		glTexImage2D(
			GL_TEXTURE_2D, 
			0,									 // mip map level
			GL_RGBA8,					// texture data 의 format ex) 1byte for each channel
			m_Specification.Width ,  // width of texture image
			m_Specification.Height,  // height of texture image
			0,									// Border width
			GL_RGBA,						// Format of pixel data
			GL_UNSIGNED_BYTE,		// Data type of pixel data
			nullptr);							// pointer to pixel data		: 마지막 3개 인자는 image 가 메모리 상에서 어떻게 표현되는지를 정의한다.

		/*
		glTextureParameteri 와 glTexParameteri 함수의 차이

		1) glTextureParameteri : operates directly on a named texture object, without requiring explicit binding.
		2) glTexParameteri       : operates on the currently bound texture object in the OpenGL context 
											and follows the traditional OpenGL binding paradigm.

											즉, glTextureParameteri 은 첫번째 인자로 내가 수정하고자 하는 texture object ID 를 넘겨줘서
											그 녀석을 조작하는 것이라면
											glTexParameteri 함수는 현재 FrameBuffer 에 Binding 된 Color Buffer 를 조작하는 것이다.
											내가 조작하고자 하는 대상을 선택할 수 없다는 것이 유일한 차이점.
											성능 차이도 없다. 
		*/

		glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
		glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		// attach a texture image to a framebuffer object
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,					// Specifies the framebuffer target
			// GL_DEPTH_ATTACHMENT		:  depth texture 를 framebuffer 객체에 첨부
			// GL_STENCIL_ATTACHMENT	:  stencil texture 
			GL_COLOR_ATTACHMENT0,	// attachment :  specifies whether the texture image should be attached to the framebuffer object's color, depth, or stencil buffer
			GL_TEXTURE_2D,
			m_ColorAttachment,				// texture : Specifies the texture object whose image is to be attached
			0);										// mip map level


		// Depth Buffer
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

		glTexStorage2D(GL_TEXTURE_2D, 
			1, 
			GL_DEPTH24_STENCIL8, 
			m_Specification.Width, 
			m_Specification.Height);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER,					// Specifies the framebuffer target
			// GL_DEPTH_ATTACHMENT		:  depth texture 를 framebuffer 객체에 첨부
			// GL_STENCIL_ATTACHMENT	:  stencil texture 
			GL_DEPTH_STENCIL_ATTACHMENT,	// attachment :  specifies whether the texture image should be attached to the framebuffer object's color, depth, or stencil buffer
			GL_TEXTURE_2D,
			m_DepthAttachment,				// texture : Specifies the texture object whose image is to be attached
			0);										// mip map level


		// Check
		HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is complete");


		// Unbind custom frameBuffer
		// -> 기본 frame buffer 를 활성화 한다.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// 해당 함수는, 이제부터 해당 FrameBuffer 에 그리겠다는 의미이다.
		// 그렇다면 우리의 ViewPort 또한 FrameBuffer 의 Size 에 맞게 그리면 된다.
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);

	}

	void OpenGLFrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width <= 0 || height <= 0 || width >= s_MaxFrameBufferSize || height >= s_MaxFrameBufferSize)
		{
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}
}

