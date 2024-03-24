#include "Hazel/Scene/Component/Renderer/SpriteRenderComponent.h"
#include "Hazel/Core/Serialization/Serializer.h"
#include "hzpch.h"

static std::string emptyTexture = "";

namespace Hazel
{
SpriteRenderComponent::SpriteRenderComponent()
{
    Reflection::RegistType<SpriteRenderComponent>();
}

SpriteRenderComponent::SpriteRenderComponent(
    const SpriteRenderComponent &other)
    : m_Color(other.m_Color), m_TilingFactor(other.m_TilingFactor)
{
    Reflection::RegistType<SpriteRenderComponent>();
}

void SpriteRenderComponent::Serialize(Serializer *serializer)
{
    serializer->BeginSaveMap(Reflection::GetTypeID<SpriteRenderComponent>(),
                             this);

    TypeInfo *compTypeInfo = Reflection::GetTypeInfo(GetType());

    serializer->Save("compName", compTypeInfo->m_Name.c_str());

    serializer->Save("texturePath",
                     m_Texture ? m_Texture->GetPath() : emptyTexture);

    serializer->Save("tilingFactor", m_TilingFactor);

    serializer->Save("color", m_Color);

    serializer->EndSaveMap();
}

void SpriteRenderComponent::Deserialize(Serializer *serializer)
{
    serializer->BeginLoadMap(Reflection::GetTypeID<SpriteRenderComponent>(),
                             this);

    std::string compName;
    serializer->Load("compName", compName);

    std::string texturePath;
    serializer->Load("texturePath", texturePath);

    if (texturePath != emptyTexture)
    {
        m_Texture = TextureManager::CreateTexture2D(texturePath);
    }

    serializer->Load("tilingFactor", m_TilingFactor);

    serializer->Load("color", m_Color);

    serializer->EndLoadMap();
}

const TypeId SpriteRenderComponent::GetType() const
{
    return Reflection::GetTypeID<SpriteRenderComponent>();
}

} // namespace Hazel