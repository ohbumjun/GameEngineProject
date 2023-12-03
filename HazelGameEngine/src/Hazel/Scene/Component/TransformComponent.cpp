#include "hzpch.h"
#include "TransformComponent.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Serialization/Serializer.h"

Hazel::TransformComponent::TransformComponent()
{
	Reflection::RegistType<TransformComponent>();
}

void Hazel::TransformComponent::Serialize(Serializer* serializer)
{
	serializer->BeginSaveMap(Reflection::GetTypeID<TransformComponent>(), this);

	serializer->Save("Translation", Translation);
	serializer->Save("Rotation", Rotation);
	serializer->Save("Scale", Scale);

	serializer->EndSaveMap();
}

void Hazel::TransformComponent::Deserialize(Serializer* serializer)
{
	serializer->BeginLoadMap(Reflection::GetTypeID<TransformComponent>(), this);

	serializer->Load("Translation", Translation);
	serializer->Load("Rotation", Rotation);
	serializer->Load("Scale", Scale);

	serializer->EndLoadMap();
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

const TypeId Hazel::TransformComponent::GetType() const
{
	return Reflection::GetTypeID<TransformComponent>();
}
