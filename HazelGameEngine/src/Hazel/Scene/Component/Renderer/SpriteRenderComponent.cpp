#include "hzpch.h"
#include "Hazel/Core/Serialization/Serializer.h"
#include "SpriteRenderComponent.h"

Hazel::SpriteRenderComponent::SpriteRenderComponent()
{
	Reflection::RegistType<SpriteRenderComponent>();
}

void Hazel::SpriteRenderComponent::Serialize(Serializer* serializer)
{
	serializer->BeginSaveMap(Reflection::GetTypeID<SpriteRenderComponent>(), this);

	Reflection::TypeInfo* compTypeInfo = Reflection::GetTypeInfo(GetType());

	serializer->Save("compName", compTypeInfo->m_Name.c_str());

	serializer->Save("color", color);

	serializer->EndSaveMap();
}

void Hazel::SpriteRenderComponent::Deserialize(Serializer* serializer)
{
	serializer->BeginLoadMap(Reflection::GetTypeID<SpriteRenderComponent>(), this);
	
	std::string compName;
	serializer->Load("compName", compName);

	serializer->Load("color", color);

	serializer->EndLoadMap();
}

const TypeId Hazel::SpriteRenderComponent::GetType() const
{
	return Reflection::GetTypeID<SpriteRenderComponent>();
}
