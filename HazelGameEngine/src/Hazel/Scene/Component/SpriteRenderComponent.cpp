﻿#include "hzpch.h"
#include "SpriteRenderComponent.h"

Hazel::SpriteRenderComponent::SpriteRenderComponent()
{
	Reflection::RegistType<SpriteRenderComponent>();
}

void Hazel::SpriteRenderComponent::Serialize(Serializer& serializer)
{
	serializer.BeginSaveMap(Reflection::GetTypeID<SpriteRenderComponent>(), this);

	serializer.Save("color", color);

	serializer.EndSaveMap();
}

void Hazel::SpriteRenderComponent::Deserialize(Serializer& serializer)
{
	serializer.BeginLoadMap(Reflection::GetTypeID<SpriteRenderComponent>(), this);

	serializer.Load("color", color);

	serializer.EndLoadMap();
}

const TypeId Hazel::SpriteRenderComponent::GetType() const
{
	return Reflection::GetTypeID<SpriteRenderComponent>();
}