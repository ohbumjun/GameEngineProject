#pragma once

#include "Renderer/Texture.h"

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

	private :
		uint32_t m_Width;
		uint32_t m_Height;
		std::string m_Path;
		uint32_t m_RendererID;
	};

}


