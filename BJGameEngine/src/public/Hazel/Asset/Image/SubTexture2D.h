#pragma once
#include "Texture.h"
#include <glm/glm.hpp>

namespace Hazel
{
class HAZEL_API SubTexture2D
{
public:
    // texture : 해당 subTexture 를 감싸고 있는 대상
    SubTexture2D(const Ref<Texture2D> &texture,
                 const glm::vec2 &min,
                 const glm::vec2 &max);

    const Ref<Texture2D> GetTexture() const
    {
        return m_Texture;
    }
    const glm::vec2 *GetTexCoords() const
    {
        return m_TexCoords;
    }

    /*
		cellSize			: 기준이 되는 sprite size
		spriteSize		:  각 Grid 의 크기 ex) cellSize 128, 128 일 때, 128, 256 짜리 크기 Sprite 를 표현하기 위해 spriteSize 는 1,2 가 된다.
		*/
    static Ref<SubTexture2D> CreateFromCoords(
        const Ref<Texture2D> &texture,
        const glm::vec2 &coords,
        const glm::vec2 &cellSize,
        const glm::vec2 &spriteSize = {1, 1});

private:
    Ref<Texture2D> m_Texture;

    glm::vec2 m_TexCoords[4];
};
} // namespace Hazel
