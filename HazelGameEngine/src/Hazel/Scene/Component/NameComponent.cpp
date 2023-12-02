#include "hzpch.h"
#include "NameComponent.h"

void Hazel::NameComponent::Serialize(Serializer* serializer)
{
	serializer->BeginSaveMap(Reflection::GetTypeID<NameComponent>(), this);

	serializer->Save("Name", name);

	serializer->EndSaveMap();
}

void Hazel::NameComponent::Deserialize(Serializer* serializer)
{
	serializer->BeginLoadMap(Reflection::GetTypeID<NameComponent>(), this);

	serializer->Load("Name", name);

	serializer->EndLoadMap();
}
