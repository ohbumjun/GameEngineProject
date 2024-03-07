#include "Hazel/Scene/Component/Identifier/NameComponent.h"
#include "hzpch.h"

#include "Hazel/Core/Serialization/Serializer.h"

Hazel::NameComponent::NameComponent()
{
    Reflection::RegistType<NameComponent>();
}

void Hazel::NameComponent::Serialize(Serializer *serializer)
{
    serializer->BeginSaveMap(Reflection::GetTypeID<NameComponent>(), this);

    Reflection::TypeInfo *compTypeInfo = Reflection::GetTypeInfo(GetType());
    serializer->Save("compName", compTypeInfo->m_Name.c_str());

    serializer->Save("Name", name);

    serializer->EndSaveMap();
}

void Hazel::NameComponent::Deserialize(Serializer *serializer)
{
    serializer->BeginLoadMap(Reflection::GetTypeID<NameComponent>(), this);

    std::string compName;
    serializer->Load("compName", compName);

    serializer->Load("Name", name);

    serializer->EndLoadMap();
}

const TypeId Hazel::NameComponent::GetType() const
{
    return Reflection::GetTypeID<NameComponent>();
}
