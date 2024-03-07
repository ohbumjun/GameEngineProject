#include "hzpch.h"

#include "Hazel/Core/Serialization/Serializer.h"
#include "Hazel/Scene/Component/RigidBody2DComponent.h"

namespace Hazel
{
Rigidbody2DComponent::Rigidbody2DComponent()
{
    Reflection::RegistType<Rigidbody2DComponent>();
}

Rigidbody2DComponent::Rigidbody2DComponent(const Rigidbody2DComponent &other)
    : m_Type(other.m_Type), m_FixedRotation(other.m_FixedRotation)
{
    Reflection::RegistType<Rigidbody2DComponent>();
}

void Rigidbody2DComponent::Serialize(Serializer *serializer)
{
    serializer->BeginSaveMap(Reflection::GetTypeID<Rigidbody2DComponent>(),
                             this);
    TypeInfo *compTypeInfo = Reflection::GetTypeInfo(GetType());
    serializer->Save("compName", compTypeInfo->m_Name.c_str());

    int bodyType = (int)m_Type;
    serializer->Save("bodyType", bodyType);
    serializer->Save("FixedRotation", m_FixedRotation);

    serializer->EndSaveMap();
}

void Rigidbody2DComponent::Deserialize(Serializer *serializer)
{
    serializer->BeginLoadMap(Reflection::GetTypeID<Rigidbody2DComponent>(),
                             this);
    std::string compName;
    serializer->Load("compName", compName);

    int bodyType = (int)m_Type;
    serializer->Load("bodyType", bodyType);
    m_Type = (BodyType)bodyType;

    serializer->Load("FixedRotation", m_FixedRotation);

    serializer->EndLoadMap();
}

} // namespace Hazel
