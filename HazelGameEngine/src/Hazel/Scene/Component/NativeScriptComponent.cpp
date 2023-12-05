#include "hzpch.h"
#include "NativeScriptComponent.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Serialization/Serializer.h"

Hazel::NativeScriptComponent::NativeScriptComponent()
{
	Reflection::RegistType<NativeScriptComponent>();
}

void Hazel::NativeScriptComponent::Serialize(Serializer* serializer)
{
}

void Hazel::NativeScriptComponent::Deserialize(Serializer* serializer)
{
}

const TypeId Hazel::NativeScriptComponent::GetType() const
{
	return Reflection::GetTypeID<NativeScriptComponent>();
}
