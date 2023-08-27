#pragma once
#include "Texture.h"
#include <glm/glm.hpp>

namespace Hazel
{
	class SubTexture2D
	{
	public  :
		// texture : 해당 subTexture 를 감싸고 있는 대상
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const Ref<Texture2D> GetTexture() const { return m_Texture; }
		const glm::vec2* GetTexCoords() const { return m_TexCoords; }

		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& spriteSize);
	private :
		Ref<Texture2D> m_Texture;

		glm::vec2 m_TexCoords[4];
	};
}



