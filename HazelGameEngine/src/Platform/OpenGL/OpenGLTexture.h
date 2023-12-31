#pragma once

#include "Hazel/Asset/Image/Texture.h"
#include <glad/glad.h>

namespace Hazel
{
	class OpenGLTexture2D : public Texture2D
	{
	public :
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(uint32_t width, uint32_t height);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;
		virtual void Bind(uint32_t slot = 0) const override;
		virtual void SetData(void* data, uint32_t size);
		virtual bool IsLoaded() const override { return m_IsLoaded; }
		virtual const std::string& GetPath() const override { return m_Path; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		virtual bool operator == (const Texture& other) const override
		{
			return m_RendererID == other.GetRendererID();
		}

	private :
		uint32_t m_Width;
		bool m_IsLoaded = false;
		uint32_t m_Height;
		std::string m_Path;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};

}


