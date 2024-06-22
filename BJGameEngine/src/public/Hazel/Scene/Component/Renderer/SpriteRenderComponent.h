#pragma once


#include "Hazel/Resource/Image/Texture.h"
#include "Hazel/Scene/Component/Component.h"

class Serializer;

namespace Hazel
{
class HAZEL_API SpriteRenderComponent : public Component
{
    friend class Scene;

public:
    SpriteRenderComponent();
    SpriteRenderComponent(const SpriteRenderComponent &other);
    SpriteRenderComponent(const glm::vec4 &color) : m_Color(color)
    {
        SpriteRenderComponent();
    }

    virtual void Serialize(Serializer *serializer);
    virtual void Deserialize(Serializer *serializer);

    operator const glm::vec4 &() const
    {
        return m_Color;
    }
    operator glm::vec4 &()
    {
        return m_Color;
    }

    void SetTexture(const Ref<Texture2D> &texture)
    {
        m_Texture = texture;
    }

    const glm::vec4 &GetColor()
    {
        return m_Color;
    }
    glm::vec4 &GetColorRef()
    {
        return m_Color;
    }
    float GetTilingFactor()
    {
        return m_TilingFactor;
    }
    float &GetTilingFactorRef()
    {
        return m_TilingFactor;
    }
    const Ref<Texture2D> &GetTexture()
    {
        return m_Texture;
    }
    virtual const TypeId GetType() const;

private:
    Ref<Texture2D> m_Texture;
    float m_TilingFactor = 1.0f;
    glm::vec4 m_Color = {1.f, 1.f, 1.f, 1.f};
};
}; // namespace Hazel
