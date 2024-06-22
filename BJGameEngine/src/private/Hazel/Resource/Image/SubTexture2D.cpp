#include "Hazel/Resource/Image/SubTexture2D.h"
#include "hzpch.h"

namespace Hazel
{
SubTexture2D::SubTexture2D(const Ref<Texture2D> &texture,
                           const glm::vec2 &min,
                           const glm::vec2 &max)
    : m_Texture(texture)
{
    m_TexCoords[0] = {min.x, min.y}; // 왼,아
    m_TexCoords[1] = {max.x, min.y}; // 오.아
    m_TexCoords[2] = {max.x, max.y}; // 오.위
    m_TexCoords[3] = {min.x, max.y}; // 왼.위
}
Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D> &texture,
                                                 const glm::vec2 &coords,
                                                 const glm::vec2 &cellSize,
                                                 const glm::vec2 &spriteSize)
{
    // 왼 아
    glm::vec2 min = {(coords.x * cellSize.x) / texture->GetWidth(),
                     (coords.y * cellSize.y) / texture->GetHeight()};

    // 오 위
    glm::vec2 max = {
        ((coords.x + spriteSize.x) * cellSize.x) / texture->GetWidth(),
        ((coords.y + spriteSize.y) * cellSize.y) / texture->GetHeight()};

    return CreateRef<SubTexture2D>(texture, min, max);
}
} // namespace Hazel
