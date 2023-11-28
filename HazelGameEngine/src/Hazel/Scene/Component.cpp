#include "hzpch.h"

#include "Component.h"
#include "Hazel/Core/Serialization/Serializer.h"

void Hazel::NameComponent::Serialize(Serializer& serializer)
{
	serializer.BeginSaveMap(Reflection::GetTypeID<NameComponent>(), this);

	serializer.Save("Name", name);

	serializer.EndSaveMap();
}

void Hazel::NameComponent::Deserialize(Serializer& serializer)
{
}

void Hazel::TransformComponent::Serialize(Serializer& serializer)
{
	serializer.BeginSaveMap(Reflection::GetTypeID<TransformComponent>(), this);

	// serializer.Save("Translation", name);

	serializer.EndSaveMap();
}

void Hazel::TransformComponent::Deserialize(Serializer& serializer)
{
}

glm::mat4 Hazel::TransformComponent::GetTransform() const
{
	// x,y,z 회전 적용한 quartenion 형태의 값 가져오기
	glm::mat4 rotation = glm::rotate(glm::mat4(1.f), Rotation.x, { 1, 0, 0 })
		* glm::rotate(glm::mat4(1.f), Rotation.y, { 0, 1, 0 })
		* glm::rotate(glm::mat4(1.f), Rotation.z, { 0, 0, 1 });

	// T * R * S
	return glm::translate(glm::mat4(1.f), Translation)
		* rotation
		* glm::scale(glm::mat4(1.f), Scale);
}

void Hazel::SpriteRenderComponent::Serialize(Serializer& serializer)
{
	serializer.BeginSaveMap(Reflection::GetTypeID<SpriteRenderComponent>(), this);

	serializer.EndSaveMap();
}

void Hazel::SpriteRenderComponent::Deserialize(Serializer& serializer)
{
}

void Hazel::CameraComponent::Serialize(Serializer& serializer)
{
	serializer.BeginSaveMap(Reflection::GetTypeID<CameraComponent>(), this);

	serializer.EndSaveMap();
}

void Hazel::CameraComponent::Deserialize(Serializer& serializer)
{
}

