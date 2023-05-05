#include "hzpch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Hazel
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) :
		m_Path(path)
	{
		int width, height, channels;

		// OPENGL 은 Texture Coord 가 아래 -> 위 방향으로 증가한다고 계산
		// 하지만 stbl 은 위에서 아래 방향으로 증가한다고 계산
		// 따라서 그 값들을 뒤집어 줘야 한다.
		stbi_set_flip_vertically_on_load(1);

		// channels : 한 픽셀에 몇개의 채널이 존재하는지 ex) rgb, rgba -> 각각 3개, 4개
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		
		HZ_CORE_ASSERT(data, "Failed to load image");
		
		m_Width = width; 
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;

		switch (channels)
		{
			case 3 :
			{
				// rgba : format / 8 : bits for each channel
				internalFormat = GL_RGB8;
				dataFormat     = GL_RGB;
			}
			break;
			case 4 :
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			break;
		}

		HZ_CORE_ASSERT(internalFormat && dataFormat, "format should not be 0");


		// 잘못 읽거나 (a 를 r 로 읽거나) / overflow (더 많은 데이터 제공)

		// buffer data 를 gpu 가 인식할 수 있는 형태로 만들기
		// + 만들어낸 Texture Object 를 가리키는 ID 리턴
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		// gpu 쪽에 Texture Buffer 가 들어갈 메모리 할당
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		// gpu 쪽에 넘겨주기 
		// - texture 가 원래 크기보다 smaller 하게 display 될때, Linear Interpolation 적용
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		// - texture 가 원래 크기보다 크게 하게 display 될때,  Neareset Interpolation 적용
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// texture data 의 일부분을 update 하는 함수
		// - m_RendererID : Update 할 Texture Object
		// - Texture Level : 0
		// - Position where update should begin : (0,0)
		// - Region being updated : m_Width, m_Height
		// - Data Type
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
	
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


