#include "hzpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Hazel
{
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	// State is not valid, so recreate it
	void OpenGLFrameBuffer::Invalidate()
	{
		// Used to create framebuffer objects
		// A framebuffer is a memory buffer that stores pixel data for display
		// - allows you to perform various rendering operations and manipulations before displaying the final result on the screen
		glCreateFramebuffers(1, &m_RendererID);

		// texture object 를 만들어주는 함수
		// + 만들어낸 Texture Object 를 가리키는 ID 리턴
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);

		// glTexImage2D : texture object 에 2D texture 를 명시하거나 load 할 때 사용하는 함수로
		//							pixel data 를 texture 에 upload 할 수 있고, 해당 texture 의 다양한 속성 ex) format, mipmapping 등을 정의할 수 있게 한다.
		//						- image, pixel data 를 그래픽 카드 메모리에 upload 한다 (cpu -> gpu)
		//						- 해당 image 를 만든 이후, rendering 에 사용할 수 있다. ex) texture 를 geometry 표면에 입히기
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, // mip map level
			GL_RGBA8,  // texture data 의 format ex) 1byte for each channel
			m_Specification.Width ,  // width of texture image
			m_Specification.Height,  // height of texture image
			0,									// Border width
			GL_RGBA,						// Format of pixel data
			GL_UNSIGNED_BYTE,		// Data type of pixel data
			nullptr);							// pointer to pixel data


		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT10,
			GL_TEXTURE_2D,
			m_ColorAttachment,
			0);
	}

}

