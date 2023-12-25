#include "hzpch.h"
#include "IDComponent.h"
#include "Hazel/Core/Serialization/Serializer.h"

Hazel::IDComponent::IDComponent(UUID uuid) :
	m_ID(uuid)
{
	Reflection::RegistType<IDComponent>();
}

Hazel::IDComponent::IDComponent(const IDComponent&)
{
	Reflection::RegistType<IDComponent>();
}

const TypeId Hazel::IDComponent::GetType() const
{
	return Reflection::GetTypeID<IDComponent>();
}

void Hazel::IDComponent::Serialize(Serializer* serializer)
{
	serializer->BeginSaveMap(Reflection::GetTypeID<IDComponent>(), this);

	serializer->Save("UUID",m_ID);

	serializer->EndSaveMap();
}

void Hazel::IDComponent::Deserialize(Serializer* serializer)
{
	serializer->BeginLoadMap(Reflection::GetTypeID<IDComponent>(), this);

	uint64_t uuid = 0;
	serializer->Load("UUID", uuid);

	m_ID = uuid;

	serializer->EndLoadMap();
}
