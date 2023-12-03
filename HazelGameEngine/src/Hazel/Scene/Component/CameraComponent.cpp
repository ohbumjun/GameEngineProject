#include "hzpch.h"
#include "CameraComponent.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Serialization/Serializer.h"

Hazel::CameraComponent::CameraComponent()
{
	Reflection::RegistType<CameraComponent>();
}

void Hazel::CameraComponent::Serialize(Serializer* serializer)
{
	serializer->BeginSaveMap(Reflection::GetTypeID<CameraComponent>(), this);

	serializer->SaveKey("Camera");
	camera.Serialize(serializer);

	serializer->Save("isPrimary", isPrimary);

	serializer->Save("isFixedAspectRatio", isFixedAspectRatio);

	serializer->EndSaveMap();
}

void Hazel::CameraComponent::Deserialize(Serializer* serializer)
{
	serializer->BeginLoadMap(Reflection::GetTypeID<CameraComponent>(), this);

	serializer->SaveKey("Camera");

	camera.Deserialize(serializer);

	serializer->Load("isPrimary", isPrimary);

	serializer->Load("isFixedAspectRatio", isFixedAspectRatio);

	serializer->EndLoadMap();
}

const TypeId Hazel::CameraComponent::GetType() const
{
	return Reflection::GetTypeID<CameraComponent>();
}

