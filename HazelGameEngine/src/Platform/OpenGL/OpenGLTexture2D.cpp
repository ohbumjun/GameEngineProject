#include "hzpch.h"
#include "OpenGLTexture2D.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Hazel
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) :
		m_Path(path)
	{
		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		
		HZ_CORE_ASSERT(data, "Failed to load image");
		
		m_Width = width; 
		m_Height = height;

		// buffer data 를 gpu 가 인식할 수 있는 형태로 만들기
		// + 만들어낸 Texture Object 를 가리키는 ID 리턴
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		// gpu 쪽에 Texture Buffer 가 들어갈 메모리 할당
		glTextureStorage2D(m_RendererID, 1, GL_RGB8, m_Width, m_Height);

		// gpu 쪽에 넘겨주기 
		// - texture 가 원래 크기보다 smaller 하게 display 될때, Linear Interpolation 적용
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		// - texture 가 원래 크기보다 크게 하게 display 될때, Linear Interpolation 적용
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// texture data 의 일부분을 update 하는 함수
		// - m_RendererID : Update 할 Texture Object
		// - Texture Level : 0
		// - Position where update should begin : (0,0)
		// - Region being updated : m_Width, m_Height
		// - Data Type
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data);
	
		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	uint32_t OpenGLTexture2D::GetWidth() const
	{
		return m_Width;
	}
	uint32_t OpenGLTexture2D::GetHeight() const
	{
		return m_Height;
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}


