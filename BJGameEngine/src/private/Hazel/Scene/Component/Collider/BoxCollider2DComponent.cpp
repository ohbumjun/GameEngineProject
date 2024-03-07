#include "Hazel/Scene/Component/Collider/BoxCollider2DComponent.h"
#include "Hazel/Core/Serialization/Serializer.h"
#include "hzpch.h"

Hazel::BoxCollider2DComponent::BoxCollider2DComponent()
{
    Reflection::RegistType<BoxCollider2DComponent>();
}

Hazel::BoxCollider2DComponent::BoxCollider2DComponent(
    const BoxCollider2DComponent &)
{
    Reflection::RegistType<BoxCollider2DComponent>();
}

void Hazel::BoxCollider2DComponent::Serialize(Serializer *serializer)
{
    serializer->BeginSaveMap(Reflection::GetTypeID<BoxCollider2DComponent>(),
                             this);
    TypeInfo *compTypeInfo = Reflection::GetTypeInfo(GetType());
    serializer->Save("compName", compTypeInfo->m_Name.c_str());

    serializer->Save("Offset", m_Offset);
    serializer->Save("Size", m_Size);

    serializer->Save("Density", m_Density);
    serializer->Save("Friction", m_Friction);
    serializer->Save("Restitution", m_Restitution);
    serializer->Save("RestitutionThreshold", m_RestitutionThreshold);

    serializer->EndSaveMap();
}

void Hazel::BoxCollider2DComponent::Deserialize(Serializer *serializer)
{
    serializer->BeginLoadMap(Reflection::GetTypeID<BoxCollider2DComponent>(),
                             this);
    std::string compName;
    serializer->Load("compName", compName);

    serializer->Load("Offset", m_Offset);
    serializer->Load("Size", m_Size);

    serializer->Load("Density", m_Density);
    serializer->Load("Friction", m_Friction);
    serializer->Load("Restitution", m_Restitution);
    serializer->Load("RestitutionThreshold", m_RestitutionThreshold);


    serializer->EndLoadMap();
}
